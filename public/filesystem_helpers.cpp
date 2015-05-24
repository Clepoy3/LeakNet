//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "tier0/dbg.h"
#include "vstdlib/strtools.h"

#include "filesystem.h"
#include "filesystem_helpers.h"
#include "characterset.h"

//-----------------------------------------------------------------------------
// Called in every FS_* function to ensure FS_InitFilesystem has been called
//-----------------------------------------------------------------------------
#define ASSERT_FS_INITED() AssertMsg(s_pFileSystem, "FS_InitFilesystem must be called before this function!")
#define ASSERT_FULL_FS_INITED() AssertMsg(s_pFullFileSystem, "FS_InitFilesystem must be called with a full filesystem before this function!")

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static IBaseFileSystem *s_pFileSystem = NULL;
static IFileSystem *s_pFullFileSystem = NULL;

// wordbreak parsing set
static characterset_t	g_BreakSet, g_BreakSetIncludingColons;

static void InitializeCharacterSets()
{
	static bool s_CharacterSetInitialized = false;
	if (!s_CharacterSetInitialized)
	{
		CharacterSetBuild( &g_BreakSet, "{}()'" );
		CharacterSetBuild( &g_BreakSetIncludingColons, "{}()':" );
		s_CharacterSetInitialized = true;
	}
}


const char* ParseFile( const char* pFileBytes, char* pToken, bool* pWasQuoted )
{
	if (pWasQuoted)
		*pWasQuoted = false;

	if (!pFileBytes)
		return 0;

	InitializeCharacterSets();

	// YWB:  Ignore colons as token separators in COM_Parse
	static bool com_ignorecolons = false;  
	characterset_t& breaks = com_ignorecolons ? g_BreakSet : g_BreakSetIncludingColons;
	
	int c;
	int len = 0;
	pToken[0] = 0;
	
// skip whitespace
skipwhite:

	while ( (c = *pFileBytes) <= ' ')
	{
		if (c == 0)
			return 0;                    // end of file;
		pFileBytes++;
	}
	
// skip // comments
	if (c=='/' && pFileBytes[1] == '/')
	{
		while (*pFileBytes && *pFileBytes != '\n')
			pFileBytes++;
		goto skipwhite;
	}
	
// skip c-style comments
	if (c=='/' && pFileBytes[1] == '*' )
	{
		// Skip "/*"
		pFileBytes += 2;

		while ( *pFileBytes  )
		{
			if ( *pFileBytes == '*' &&
				 pFileBytes[1] == '/' )
			{
				pFileBytes += 2;
				break;
			}

			pFileBytes++;
		}

		goto skipwhite;
	}

// handle quoted strings specially
	if (c == '\"')
	{
		if (pWasQuoted)
			*pWasQuoted = true;

		pFileBytes++;
		while (1)
		{
			c = *pFileBytes++;
			if (c=='\"' || !c)
			{
				pToken[len] = 0;
				return pFileBytes;
			}
			pToken[len] = c;
			len++;
		}
	}

// parse single characters
	if ( IN_CHARACTERSET( breaks, c ) )
	{
		pToken[len] = c;
		len++;
		pToken[len] = 0;
		return pFileBytes+1;
	}

// parse a regular word
	do
	{
		pToken[len] = c;
		pFileBytes++;
		len++;
		c = *pFileBytes;
		if ( IN_CHARACTERSET( breaks, c ) )
			break;
	} while (c>32);
	
	pToken[len] = 0;
	return pFileBytes;
}


char* ParseFile( char* pFileBytes, char* pToken, bool* pWasQuoted )
{
	return (char*)ParseFile( (const char*)pFileBytes, pToken, pWasQuoted );
}

//-----------------------------------------------------------------------------
// Purpose: initializes all other FS_* functions for use
//-----------------------------------------------------------------------------
void FS_InitFilesystem( IBaseFileSystem *pFS )
{
	AssertMsg(!s_pFileSystem, "FS_InitFilesystem called more than once");

	Assert(pFS);
	s_pFileSystem = pFS;
}

void FS_InitFilesystem( IFileSystem *pFS )
{
	AssertMsg(!s_pFullFileSystem, "FS_InitFilesystem called more than once");

	Assert(pFS);
	s_pFileSystem = s_pFullFileSystem = pFS;
}

//-----------------------------------------------------------------------------
// Purpose: read the contents of pszFile into buf
// Output : true = file was read into buf successfully
//			false = file wasn't read completely into buf
//-----------------------------------------------------------------------------
bool FS_ReadFile( const char *pszFile, CUtlBuffer &buf, const char *pathID/*=NULL*/ )
{
//	ASSERT_FS_INITED();
	Assert(pszFile);

	// Try to open file
	FileHandle_t hFile = s_pFileSystem->Open(pszFile, "rb", pathID);

	if(!hFile)
	{
		Warning("FS_ReadFile: unable to load file %s\n", pszFile);
		return false;
	}

	// Get file size
	unsigned int nByteSize = s_pFileSystem->Size(hFile);

	// Ensure enough space in buffer
	buf.EnsureCapacity(nByteSize+1);
	memset(buf.Base(), 0, nByteSize+1); ///< clear buffer (will null terminate)

	// Read data into buffer
	s_pFileSystem->Read(buf.Base(), nByteSize, hFile);

	// Close file
	s_pFileSystem->Close(hFile);

	return true;
}

