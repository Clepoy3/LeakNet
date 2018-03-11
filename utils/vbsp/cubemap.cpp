//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "vbsp.h"
#include "bsplib.h"
#include "UtlBuffer.h"
#include "UtlVector.h"
#include "ImageLoader.h"
#include <KeyValues.h>
#include "vstdlib/strtools.h"
#include "utlsymbol.h"
#include "vtf/vtf.h"
#include "materialpatch.h"

struct CubemapInfo_t
{
	int m_nTableId;
	bool m_bSpecular;
};

static bool CubemapLessFunc( const CubemapInfo_t &lhs, const CubemapInfo_t &rhs )
{ 
	return ( lhs.m_nTableId < rhs.m_nTableId );	
}

static CUtlVector<char *> s_DefaultCubemapNames;

void Cubemap_InsertSample( const Vector& origin, int size )
{
	dcubemapsample_t *pSample = &g_CubemapSamples[g_nCubemapSamples];
	pSample->origin[0] = ( int )origin[0];	
	pSample->origin[1] = ( int )origin[1];	
	pSample->origin[2] = ( int )origin[2];	
	pSample->size = size;
	g_nCubemapSamples++;
}

static const char *FindSkyboxMaterialName( void )
{
	for( int i = 0; i < num_entities; i++ )
	{
		char* pEntity = ValueForKey(&entities[i], "classname");
		if (!strcmp(pEntity, "worldspawn"))
		{
			return ValueForKey( &entities[i], "skyname" );
		}
	}
	return NULL;
}

static void BackSlashToForwardSlash( char *pname )
{
	while ( *pname ) {
		if ( *pname == '\\' )
			*pname = '/';
		pname++;
	}
}

static void ForwardSlashToBackSlash( char *pname )
{
	while ( *pname ) {
		if ( *pname == '/' )
			*pname = '\\';
		pname++;
	}
}

//-----------------------------------------------------------------------------
// Finds materials that are used by a particular material
//-----------------------------------------------------------------------------
#define MAX_MATERIAL_NAME 512

// This is the list of materialvars which are used in our codebase to look up dependent materials
static const char *s_pDependentMaterialVar[] = 
{
	"$bottommaterial",	// Used by water materials
	"$crackmaterial",	// Used by shattered glass materials
	"$fallbackmaterial",	// Used by all materials

	"",					// Always must be last
};


// VXP: Used in DoesMaterialOrDependentsUseEnvmap (as for fixing cubemaps in vbsp),
// and also in PatchEnvmapForMaterialAndDependents (patching non env_cubemap sides - already done in leak code now by me)
static const char *FindDependentMaterial( const char *pMaterialName, const char **ppMaterialVar = NULL )
{
	// FIXME: This is a terrible way of doing this! It creates a dependency
	// between vbsp and *all* code which reads dependent materials from materialvars
	// At the time of writing this function, that means the engine + studiorender.
	// We need a better way of figuring out how to do this, but for now I'm trying to do
	// the fastest solution possible since it's close to ship

	static char pDependentMaterialName[MAX_MATERIAL_NAME];
	for( int i = 0; s_pDependentMaterialVar[i][0]; ++i )
	{
		if ( !GetValueFromMaterial( pMaterialName, s_pDependentMaterialVar[i], pDependentMaterialName, MAX_MATERIAL_NAME - 1 ) )
			continue;

		if ( !Q_stricmp( pDependentMaterialName, pMaterialName ) )
		{
			Warning( "Material %s is depending on itself through materialvar %s! Ignoring...\n", pMaterialName, s_pDependentMaterialVar[i] );
				continue;
		}

		// Return the material var that caused the dependency
		if ( ppMaterialVar )
		{
			*ppMaterialVar = s_pDependentMaterialVar[i];
		}

#ifdef _DEBUG
		// FIXME: Note that this code breaks if a material has more than 1 dependent material
		++i;
		static char pDependentMaterialName2[MAX_MATERIAL_NAME];
		while( s_pDependentMaterialVar[i][0] )
		{
			Assert( !GetValueFromMaterial( pMaterialName, s_pDependentMaterialVar[i], pDependentMaterialName2, MAX_MATERIAL_NAME - 1 ) );
			++i;
		}
#endif

		return pDependentMaterialName;
	}

	return NULL;
}

static bool LoadSrcVTFFiles( IVTFTexture *pSrcVTFTextures[6], const char *pSkyboxBaseName )
{
	const char *facingName[6] = { "rt", "lf", "bk", "ft", "up", "dn" };
	int i;
	for( i = 0; i < 6; i++ )
	{
		char srcVTFFileName[1024];
		sprintf( srcVTFFileName, "%smaterials/skybox/%s%s.vtf", gamedir, pSkyboxBaseName, facingName[i] );

		FILE *fp;
		fp = fopen( srcVTFFileName, "rb" );
		if( !fp )
		{
			sprintf( srcVTFFileName, "%smaterials/skybox/%s%s.vtf", basegamedir, pSkyboxBaseName, facingName[i] );
			fp = fopen( srcVTFFileName, "rb" );
			if ( !fp ) // VXP: Dirty hack - trying to find skybox texture in base hl2 folder
			{
				Warning( "*** Error opening skybox texture: %s\n", srcVTFFileName );
				return false;
			}
		}
		fseek( fp, 0, SEEK_END );
		int srcVTFLength = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		CUtlBuffer buf;
		buf.EnsureCapacity( srcVTFLength );
		fread( buf.Base(), 1, srcVTFLength, fp );
		fclose( fp );

		pSrcVTFTextures[i] = CreateVTFTexture();
		if (!pSrcVTFTextures[i]->Unserialize(buf))
		{
			Warning("*** Error unserializing skybox texture: %s\n", pSkyboxBaseName );
			return false;
		}

		if ( (pSrcVTFTextures[i]->Width() != pSrcVTFTextures[0]->Width()) ||
			 (pSrcVTFTextures[i]->Flags() != pSrcVTFTextures[0]->Flags()) )
		{
			Warning("*** Error: Skybox vtf files for %s weren't compiled with the same size texture and/or same flags!\n", pSkyboxBaseName );
			return false;
		}
	}

	return true;
}

#define DEFAULT_CUBEMAP_SIZE 32

void Cubemap_CreateDefaultCubemaps( void )
{
	// NOTE: This implementation depends on the fact that all VTF files contain
	// all mipmap levels
	const char *pSkyboxBaseName = FindSkyboxMaterialName();

	IVTFTexture *pSrcVTFTextures[6];

	if( !pSkyboxBaseName )
	{
		if( s_DefaultCubemapNames.Count() )
		{
			Warning( "This map uses env_cubemap, and you don't have a skybox, so no default env_cubemaps will be generated.\n" );
		}
		return;
	}

	if( !LoadSrcVTFFiles( pSrcVTFTextures, pSkyboxBaseName ) )
	{
		Warning( "Can't load skybox file %s to build the default cubemap!\n", pSkyboxBaseName );
		return;
	}
	Msg( "Creating default cubemaps for env_cubemap using skybox %s...\n"
		"Run buildcubemaps in the engine to get the correct cube maps.\n", pSkyboxBaseName );
			
	// Figure out the mip differences between the two textures
	int iMipLevelOffset = 0;
	int tmp = pSrcVTFTextures[0]->Width();
	while( tmp > DEFAULT_CUBEMAP_SIZE )
	{
		iMipLevelOffset++;
		tmp >>= 1;
	}

	// Create the destination cubemap
	IVTFTexture *pDstCubemap = CreateVTFTexture();
	pDstCubemap->Init( DEFAULT_CUBEMAP_SIZE, DEFAULT_CUBEMAP_SIZE, 
		pSrcVTFTextures[0]->Format(), pSrcVTFTextures[0]->Flags() | TEXTUREFLAGS_ENVMAP, 
		pSrcVTFTextures[0]->FrameCount() );

	// First iterate over all frames
	for (int iFrame = 0; iFrame < pDstCubemap->FrameCount(); ++iFrame)
	{
		// Next iterate over all normal cube faces (we know there's 6 cause it's an envmap)
		for (int iFace = 0; iFace < 6; ++iFace )
		{
			// Finally, iterate over all mip levels in the *destination*
			for (int iMip = 0; iMip < pDstCubemap->MipCount(); ++iMip )
			{
				// Copy the bits from the source images into the cube faces
				unsigned char *pSrcBits = pSrcVTFTextures[iFace]->ImageData( iFrame, 0, iMip + iMipLevelOffset );
				unsigned char *pDstBits = pDstCubemap->ImageData( iFrame, iFace, iMip );
				int iSize = pDstCubemap->ComputeMipSize( iMip );

				memcpy( pDstBits, pSrcBits, iSize ); 
			}
		}
	}

	// Convert the cube to format that we can apply tools to it...
	ImageFormat originalFormat = pDstCubemap->Format();
	pDstCubemap->ConvertImageFormat( IMAGE_FORMAT_DEFAULT, false );

	// Fixup the cubemap facing
	pDstCubemap->FixCubemapFaceOrientation();

	// Now that the bits are in place, compute the spheremaps...
	pDstCubemap->GenerateSpheremap();

	// Convert the cubemap to the final format
	pDstCubemap->ConvertImageFormat( originalFormat, false );

	// Write the puppy out!
	char dstVTFFileName[1024];
	sprintf( dstVTFFileName, "materials/maps/%s/cubemapdefault.vtf", mapbase );

	CUtlBuffer outputBuf;
	if (!pDstCubemap->Serialize( outputBuf ))
	{
		Warning( "Error serializing default cubemap %s\n", dstVTFFileName );
		return;
	}

	// stpie out the default one.
	AddBufferToPack( dstVTFFileName, outputBuf.Base(), outputBuf.TellPut(), false );

	// spit out all of the ones that are attached to world geometry.
	int i;
	for( i = 0; i < s_DefaultCubemapNames.Count(); i++ )
	{
		AddBufferToPack( s_DefaultCubemapNames[i], outputBuf.Base(),outputBuf.TellPut(), false );
	}

	// Clean up the textures
	for( i = 0; i < 6; i++ )
	{
		DestroyVTFTexture( pSrcVTFTextures[i] );
	}
	DestroyVTFTexture( pDstCubemap );
}	

typedef CUtlVector<int> IntVector_t;
static CUtlVector<IntVector_t> s_EnvCubemapToBrushSides;


struct CubemapSideData_t
{
	bool bHasEnvMapInMaterial;
	bool bManuallyPickedByAnEnvCubemap;
};
static CubemapSideData_t s_aCubemapSideData[MAX_MAP_BRUSHSIDES];

inline bool SideHasCubemapAndWasntManuallyReferenced( int iSide )
{
	return s_aCubemapSideData[iSide].bHasEnvMapInMaterial && !s_aCubemapSideData[iSide].bManuallyPickedByAnEnvCubemap;
}


void Cubemap_SaveBrushSides( const char *pSideListStr )
{
	int iCubemapID = s_EnvCubemapToBrushSides.AddToTail();
//	Warning( "Saving %i cubemap for these sides: \"%s\"\n", iCubemapID, pSideListStr );
	IntVector_t &brushSidesVector = s_EnvCubemapToBrushSides[iCubemapID];
	char *pTmp = ( char * )_alloca( strlen( pSideListStr ) + 1 );
	strcpy( pTmp, pSideListStr );
	const char *pScan = strtok( pTmp, " " );
	if( !pScan )
	{
	//	Warning( "\tThis cubemap doesn't have assigned brushes!\n" );
		return;
	}
	do
	{
		int brushSideID;
		if( sscanf( pScan, "%d", &brushSideID ) == 1 )
		{
			brushSidesVector.AddToTail( brushSideID );
		}
	} while( ( pScan = strtok( NULL, " " ) ) );
}

static int Cubemap_CreateTexInfo( int originalTexInfo, int origin[3] )
{
	Warning( "Cubemap_CreateTexInfo\n" );
	texinfo_t *pTexInfo = &texinfo[originalTexInfo];
	dtexdata_t *pTexData = GetTexData( pTexInfo->texdata );

	char stringToSearchFor[512];
	sprintf( stringToSearchFor, "_%d_%d_%d", origin[0], origin[1], origin[2] );

	// Get out of here if the originalTexInfo is already a generated material for this position.
	if( Q_stristr( TexDataStringTable_GetString( pTexData->nameStringTableID ),
		stringToSearchFor ) )
	{
		return originalTexInfo;
	}

	char generatedTexDataName[1024];
	sprintf( generatedTexDataName, "maps/%s/%s_%d_%d_%d", mapbase, 
		TexDataStringTable_GetString( pTexData->nameStringTableID ), 
		( int )origin[0], ( int )origin[1], ( int )origin[2] );
	BackSlashToForwardSlash( generatedTexDataName );
	static CUtlSymbolTable complainedSymbolTable( 0, 32, true );

	int texDataID = -1;
	bool hasTexData = false;
	// Make sure the texdata doesn't already exist.
	int i;
	for( i = 0; i < numtexdata; i++ )
	{
		if( stricmp( TexDataStringTable_GetString( GetTexData( i )->nameStringTableID ), 
			generatedTexDataName ) == 0 )
		{
			hasTexData = true;
			texDataID = i;
			break;
		}
	}
	
	if( !hasTexData )
	{
		char originalMatName[1024];
		char generatedMatName[1024];
		sprintf( originalMatName, "%s", 
			TexDataStringTable_GetString( pTexData->nameStringTableID ) );
		sprintf( generatedMatName, "maps/%s/%s_%d_%d_%d", mapbase, 
			TexDataStringTable_GetString( pTexData->nameStringTableID ), 
			( int )origin[0], ( int )origin[1], ( int )origin[2] );
		strlwr( generatedTexDataName );

		char envmapVal[512];
		GetValueFromMaterial( originalMatName, "$envmap", envmapVal, 511 );
		if( !GetValueFromMaterial( originalMatName, "$envmap", envmapVal, 511 ) )
		{
			if( UTL_INVAL_SYMBOL == complainedSymbolTable.Find( originalMatName ) )
			{
				Warning( "Ignoring env_cubemap on \"%s\" which doesn't use an envmap\n", originalMatName );
				complainedSymbolTable.AddString( originalMatName );
			}
			return originalTexInfo;
		}
		if( stricmp( envmapVal, "env_cubemap" ) != 0 )
		{
			if( UTL_INVAL_SYMBOL == complainedSymbolTable.Find( originalMatName ) )
			{
				Warning( "Ignoring env_cubemap on \"%s\" which uses envmap \"%s\"\n", originalMatName, envmapVal );
				complainedSymbolTable.AddString( originalMatName );
			}
			return originalTexInfo;
		}

		// Patch "$envmap"
		char textureName[1024];
		sprintf( textureName, "maps/%s/c%d_%d_%d", mapbase, ( int )origin[0], 
			( int )origin[1], ( int )origin[2] );

	//	Msg( "Cubemap_CreateTexInfo: Generating material patch...\n" );
		CreateMaterialPatch( originalMatName, generatedMatName, "$envmap",
			textureName );

		// Store off the name of the cubemap that we need to create.
		char fileName[1024];
		sprintf( fileName, "materials/%s.vtf", textureName );
		int id = s_DefaultCubemapNames.AddToTail();
		s_DefaultCubemapNames[id] = new char[strlen( fileName ) + 1];
		strcpy( s_DefaultCubemapNames[id], fileName );

		// Make a new texdata
		Assert( strlen( generatedTexDataName ) < TEXTURE_NAME_LENGTH - 1 );
		if( !( strlen( generatedTexDataName ) < TEXTURE_NAME_LENGTH - 1 ) )
		{
			Error( "generatedTexDataName: %s too long!\n", generatedTexDataName );
		}
		strlwr( generatedTexDataName );
		texDataID = AddCloneTexData( pTexData, generatedTexDataName );
		
	}

	Assert( texDataID != -1 );
	
	texinfo_t newTexInfo;
	newTexInfo = *pTexInfo;
	newTexInfo.texdata = texDataID;
	
	int texInfoID = -1;

	// See if we need to make a new texinfo
	bool hasTexInfo = false;
	if( !hasTexData )
	{
		hasTexInfo = false;
	}
	else
	{
		for( i = 0; i < numtexinfo; i++ )
		{
			if( texinfo[i].texdata != texDataID )
			{
				continue;
			}
			if( memcmp( &texinfo[i], &newTexInfo, sizeof( texinfo_t ) ) == 0 )
			{
				hasTexInfo = true;
				texInfoID = i;
				break;
			}
		}
	}
	
	// Make a new texinfo if we need to.
	if( !hasTexInfo )
	{
		if ( numtexinfo >= MAX_MAP_TEXINFO )
		{
			Error( "Map has too many texinfos, MAX_MAP_TEXINFO == %i\n", MAX_MAP_TEXINFO );
		}
		texinfo[numtexinfo] = newTexInfo;
		texInfoID = numtexinfo;
		numtexinfo++;
	}

	Assert( texInfoID != -1 );
	return texInfoID;
}

static void Cubemap_CreateMaterialForBrushSide( int sideIndex, int origin[3] )
{
	Warning( "Cubemap_CreateMaterialForBrushSide\n" );
	side_t *pSide = &brushsides[sideIndex];
	int originalTexInfoID = pSide->texinfo;
	pSide->texinfo = Cubemap_CreateTexInfo( originalTexInfoID, origin );
}

static int SideIDToIndex( int brushSideID )
{
	int i;
	for( i = 0; i < nummapbrushsides; i++ )
	{
		if( brushsides[i].id == brushSideID )
		{
			return i;
		}
	}
	Assert( 0 );
	return -1;
}

void Cubemap_FixupBrushSidesMaterials( void )
{
	Msg( "fixing up env_cubemap materials on brush sides...\n" );
	Msg( "There are %i cubemap samples on the map\n", g_nCubemapSamples );
	Assert( s_EnvCubemapToBrushSides.Count() == g_nCubemapSamples );

	int cubemapID;
	for( cubemapID = 0; cubemapID < g_nCubemapSamples; cubemapID++ )
	{
		IntVector_t &brushSidesVector = s_EnvCubemapToBrushSides[cubemapID];
		Msg( "vector count %i\n", brushSidesVector.Count() );
	//	Warning( "Cubemap_FixupBrushSidesMaterials loop\nbrushSidesVector = %i\ns_EnvCubemapToBrushSides = %i\n", brushSidesVector.Count(), s_EnvCubemapToBrushSides.Count() );
		int i;
		for( i = 0; i < brushSidesVector.Count(); i++ )
		{
		//	Warning( "Cubemap_FixupBrushSidesMaterials second loop\n" );
			int brushSideID = brushSidesVector[i];
			int sideIndex = SideIDToIndex( brushSideID );
			if( sideIndex < 0 )
			{
				Warning("env_cubemap pointing at deleted brushside near (%d, %d, %d)\n", 
					g_CubemapSamples[cubemapID].origin[0], g_CubemapSamples[cubemapID].origin[1], g_CubemapSamples[cubemapID].origin[2] );

				continue;
			}
			Cubemap_CreateMaterialForBrushSide( sideIndex, g_CubemapSamples[cubemapID].origin );
			side_t *pSide = &brushsides[sideIndex];
			texinfo_t *pTexInfo = &texinfo[pSide->texinfo];
			dtexdata_t *pTexData = GetTexData( pTexInfo->texdata );

		//	pSide->texinfo = Cubemap_CreateTexInfo( pSide->texinfo, g_CubemapSamples[cubemapID].origin ); // VXP: From Source 2007, but not needed as I can see
		}
	}
}

void Cubemap_FixupDispMaterials( void )
{
//	Warning( "fixing up env_cubemap materials on displacements...\n" );
	Assert( s_EnvCubemapToBrushSides.Count() == g_nCubemapSamples );

	// FIXME: this is order N to the gazillion!
	int cubemapID;
	for( cubemapID = 0; cubemapID < g_nCubemapSamples; cubemapID++ )
	{
		Warning( "Creating TexInfo for cubemapID %i\n", cubemapID );
		IntVector_t &brushSidesVector = s_EnvCubemapToBrushSides[cubemapID];
		int i;
		for( i = 0; i < brushSidesVector.Count(); i++ )
		{
			// brushSideID is the unique id for the side
			int brushSideID = brushSidesVector[i];
			int j;
			for( j = 0; j < numfaces; j++ )
			{
				dface_t *pFace = &dfaces[j];
				if( pFace->dispinfo == -1 )
				{
					continue;
				}
				mapdispinfo_t *pDispInfo = &mapdispinfo[pFace->dispinfo];
				if( brushSideID == pDispInfo->brushSideID )
				{
					pFace->texinfo = Cubemap_CreateTexInfo( pFace->texinfo, g_CubemapSamples[cubemapID].origin );
				}
			}
		}
	}
}

void Cubemap_MakeDefaultVersionsOfEnvCubemapMaterials( void )
{
	int i;
	char originalMaterialName[1024];
	char patchMaterialName[1024];
	char defaultVTFFileName[1024];
	sprintf( defaultVTFFileName, "maps/%s/cubemapdefault", mapbase );
	for( i = 0; i < numtexdata; i++ )
	{
		dtexdata_t *pTexData = GetTexData( i );
		sprintf( originalMaterialName, "%s", 
			TexDataStringTable_GetString( pTexData->nameStringTableID ) );
		sprintf( patchMaterialName, "maps/%s/%s", 
			mapbase,
			TexDataStringTable_GetString( pTexData->nameStringTableID ) );

		// Skip generated materials FIXME!!!
//		if( Q_stristr( originalVMTName, "materials/maps" ) )
//		{
//			continue;
//		}
		
		// Check to make sure that it has an envmap
		char oldEnvmapName[512];
		if( !GetValueFromMaterial( originalMaterialName, "$envmap", oldEnvmapName, 511 ) )
		{
			continue;
		}

		if( !g_bNoEnvmapFix && stricmp( oldEnvmapName, "env_cubemap" ) != 0 )
		{
			Warning( "Cubemap_MakeDefaultVersionsOfEnvCubemapMaterials: Ignoring env_cubemap on \"%s\" which uses envmap \"%s\"\n", originalMaterialName, oldEnvmapName );
			continue;
		}
		
	//	Msg( "Cubemap_MakeDefaultVersionsOfEnvCubemapMaterials: Generating material patch... %s, %s\n", originalMaterialName, patchMaterialName );
		CreateMaterialPatch( originalMaterialName, patchMaterialName,
							 "$envmap", defaultVTFFileName );
		// FIXME!:  Should really remove the old name from the string table
		// it it isn't used.
		
		// Fix the texdata so that it points at the new vmt.
		char newTexDataName[1024];
		sprintf( newTexDataName, "maps/%s/%s", mapbase, 
			TexDataStringTable_GetString( pTexData->nameStringTableID ) );
		pTexData->nameStringTableID = TexDataStringTable_AddOrFindString( newTexDataName );
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Cubemap_ResetCubemapSideData( void )
{
	for ( int iSide = 0; iSide < MAX_MAP_BRUSHSIDES; ++iSide )
	{
		s_aCubemapSideData[iSide].bHasEnvMapInMaterial = false;
		s_aCubemapSideData[iSide].bManuallyPickedByAnEnvCubemap = false;
	}
}

//-----------------------------------------------------------------------------
// Returns true if the material or any of its dependents use an $envmap
//-----------------------------------------------------------------------------
bool DoesMaterialOrDependentsUseEnvmap( const char *pPatchedMaterialName )
{
	const char *pOriginalMaterialName = GetOriginalMaterialNameForPatchedMaterial( pPatchedMaterialName );
	if( DoesMaterialHaveKey( pOriginalMaterialName, "$envmap" ) )
		return true;

	const char *pDependentMaterial = FindDependentMaterial( pOriginalMaterialName );
	if ( !pDependentMaterial )
		return false;

	return DoesMaterialOrDependentsUseEnvmap( pDependentMaterial );
}

//-----------------------------------------------------------------------------
// Builds a list of all texdatas which need fixing up
//-----------------------------------------------------------------------------
void Cubemap_InitCubemapSideData( void )
{
	// This tree is used to prevent re-parsing material vars multiple times
	CUtlRBTree<CubemapInfo_t> lookup( 0, nummapbrushsides, CubemapLessFunc );

	// Fill in specular data.
	for ( int iSide = 0; iSide < nummapbrushsides; ++iSide )
	{
		side_t *pSide = &brushsides[iSide];
		if ( !pSide )
			continue;

		if ( pSide->texinfo == TEXINFO_NODE )
			continue;

		texinfo_t *pTex = &texinfo[pSide->texinfo];
		if ( !pTex )
			continue;

		dtexdata_t *pTexData = GetTexData( pTex->texdata );
		if ( !pTexData )
			continue;

		CubemapInfo_t info;
		info.m_nTableId = pTexData->nameStringTableID;

		// Have we encountered this materal? If so, then copy the data we cached off before
		int i = lookup.Find( info );
		if ( i != lookup.InvalidIndex() )
		{
			s_aCubemapSideData[iSide].bHasEnvMapInMaterial = lookup[i].m_bSpecular;
			continue;
		}

		// First time we've seen this material. Figure out if it uses env_cubemap
		const char *pPatchedMaterialName = TexDataStringTable_GetString( pTexData->nameStringTableID );
		info.m_bSpecular = DoesMaterialOrDependentsUseEnvmap( pPatchedMaterialName );
		s_aCubemapSideData[ iSide ].bHasEnvMapInMaterial = info.m_bSpecular;
		lookup.Insert( info );
	}

	// Fill in cube map data.
	for ( int iCubemap = 0; iCubemap < g_nCubemapSamples; ++iCubemap )
	{
		IntVector_t &sideList = s_EnvCubemapToBrushSides[iCubemap];
		int nSideCount = sideList.Count();
		for ( int iSide = 0; iSide < nSideCount; ++iSide )
		{
			int nSideID = sideList[iSide];
			int nIndex = SideIDToIndex( nSideID );
			if ( nIndex < 0 )
				continue;

			s_aCubemapSideData[nIndex].bManuallyPickedByAnEnvCubemap = true;
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int Cubemap_FindClosestCubemap( const Vector &entityOrigin, side_t *pSide )
{
	if ( !pSide )
		return -1;

	// Return a valid (if random) cubemap if there's no winding
	if ( !pSide->winding )
		return 0;

	// Calculate the center point.
	Vector vecCenter;
	vecCenter.Init();

	for ( int iPoint = 0; iPoint < pSide->winding->numpoints; ++iPoint )
	{
		VectorAdd( vecCenter, pSide->winding->p[iPoint], vecCenter );
	}
	VectorScale( vecCenter, 1.0f / pSide->winding->numpoints, vecCenter );
	vecCenter += entityOrigin;
	plane_t *pPlane = &mapplanes[pSide->planenum];

	// Find the closest cubemap.
	int iMinCubemap = -1;
	float flMinDist = FLT_MAX;

	// Look for cubemaps in front of the surface first.
	for ( int iCubemap = 0; iCubemap < g_nCubemapSamples; ++iCubemap )
	{	
		dcubemapsample_t *pSample = &g_CubemapSamples[iCubemap];
		Vector vecSampleOrigin( static_cast<float>( pSample->origin[0] ),
								static_cast<float>( pSample->origin[1] ),
								static_cast<float>( pSample->origin[2] ) );
		Vector vecDelta;
		VectorSubtract( vecSampleOrigin, vecCenter, vecDelta );
	//	float flDist = vecDelta.NormalizeInPlace();
		float flDist = VectorNormalize( vecDelta );
		float flDot = DotProduct( vecDelta, pPlane->normal );
		if ( ( flDot >= 0.0f ) && ( flDist < flMinDist ) )
		{
			flMinDist = flDist;
			iMinCubemap = iCubemap;
		}
	}

	// Didn't find anything in front search for closest.
	if( iMinCubemap == -1 )
	{
		for ( int iCubemap = 0; iCubemap < g_nCubemapSamples; ++iCubemap )
		{	
			dcubemapsample_t *pSample = &g_CubemapSamples[iCubemap];
			Vector vecSampleOrigin( static_cast<float>( pSample->origin[0] ),
				static_cast<float>( pSample->origin[1] ),
				static_cast<float>( pSample->origin[2] ) );
			Vector vecDelta;
			VectorSubtract( vecSampleOrigin, vecCenter, vecDelta );
			float flDist = vecDelta.Length();
			if ( flDist < flMinDist )
			{
				flMinDist = flDist;
				iMinCubemap = iCubemap;
			}
		}
	}

	return iMinCubemap;
}

struct entitySideList_t
{
	int firstBrushSide;
	int brushSideCount;
};
//-----------------------------------------------------------------------------
// For every specular surface that wasn't referenced by some env_cubemap, call Cubemap_CreateTexInfo.
//-----------------------------------------------------------------------------
void Cubemap_AttachDefaultCubemapToSpecularSides( void )
{
	Cubemap_ResetCubemapSideData();
	Cubemap_InitCubemapSideData();

	CUtlVector<entitySideList_t> sideList;
	sideList.SetCount( num_entities );
	int i;
	for ( i = 0; i < num_entities; i++ )
	{
		sideList[i].firstBrushSide = 0;
		sideList[i].brushSideCount = 0;
	}

	for ( i = 0; i < nummapbrushes; i++ )
	{
		sideList[mapbrushes[i].entitynum].brushSideCount += mapbrushes[i].numsides;
	}
	int curSide = 0;
	for ( i = 0; i < num_entities; i++ )
	{
		sideList[i].firstBrushSide = curSide;
		curSide += sideList[i].brushSideCount;
	}

	int currentEntity = 0;
	for ( int iSide = 0; iSide < nummapbrushsides; ++iSide )
	{
		side_t *pSide = &brushsides[iSide];
		if ( !SideHasCubemapAndWasntManuallyReferenced( iSide ) )
			continue;

		while ( currentEntity < num_entities-1 && 
			iSide > sideList[currentEntity].firstBrushSide + sideList[currentEntity].brushSideCount )
		{
			currentEntity++;
		}

		int iCubemap = Cubemap_FindClosestCubemap( entities[currentEntity].origin, pSide );
		if ( iCubemap == -1 )
			continue;

#ifdef DEBUG
		if ( pSide->pMapDisp )
		{
			Assert( pSide->texinfo == pSide->pMapDisp->face.texinfo );
		}
#endif				
		pSide->texinfo = Cubemap_CreateTexInfo( pSide->texinfo, g_CubemapSamples[iCubemap].origin );
		if ( pSide->pMapDisp )
		{
			pSide->pMapDisp->face.texinfo = pSide->texinfo;
		}
	}
}