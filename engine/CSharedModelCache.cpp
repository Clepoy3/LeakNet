//======= Copyright © 2008-2011, The Axel Project, all rights reserved ======//
//
// Author: Saul Rennison <saul.rennison@gmail.com>
// Purpose: Shared model cache file, used by bone_setup.cpp
//
//===========================================================================//

#include "CSharedModelCache.h"
#include "utllinkedlist.h"
#include "filesystem.h"
#include "server.h"
#include "modelgen.h" // VXP

//-----------------------------------------------------------------------------
// Interface access
//-----------------------------------------------------------------------------
static CSharedModelCache s_SharedModelCache;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CSharedModelCache, ISharedModelCache, SHARED_MODEL_CACHE_INTERFACE_VERSION, s_SharedModelCache);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CSharedModelCache::InitFileSystem( IBaseFileSystem* pFileSystem )
{
	ErrorIfNot(pFileSystem, ("CSharedModelCache::InitFileSystem() passed NULL filesystem"));

	m_pFileSystem = pFileSystem;
	m_bInitted = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
studiosharehdr_t* CSharedModelCache::GetSharedModel( int nIndex )
{
	// Decrement the index by 1
	nIndex -= 1;

	// Check the index exists
	if(!m_CacheNodes.IsValidIndex(nIndex))
	{
		Warning("CSharedModelCache::GetSharedModel( %d ): not found in cache\n", nIndex);
		return NULL;
	}

	// Return the model
	SharedModelCacheNode_s& node = m_CacheNodes[nIndex];
	return node.pStudioShareHeader;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CSharedModelCache::Load( const char* pszFileLocation )
{
	ErrorIfNot(m_bInitted, ("CSharedModelCache::InitFileSystem() must be called before ::Load()"));

	if(!pszFileLocation)
	{
		Warning("CSharedModelCache::Load( <null> ): NULL file location\n");
		return Cache_Invalid;
	}

	// Warn if we are trying to Load a shared model while the server is running
#ifdef GAME_DLL
	if(sv.active)
		if(sv.state == ss_active)
			Warning("CSharedModelCache::Load( %s ): performance warning: precaching while server is running may cause hitches\n", pszFileLocation);
#endif // GAME_DLL

	// See if it's already in the cache first
	FOR_EACH_LL(m_CacheNodes, i)
	{
		if(Q_strcmp(m_CacheNodes[i].pStudioShareHeader->name, pszFileLocation) == 0)
			return i+1;
	}

	// Add a node to the cache
	unsigned int nIndex = m_CacheNodes.AddToTail();
	SharedModelCacheNode_s& node = m_CacheNodes[nIndex];
	
	// Attempt to open the file
	FileHandle_t pFileHandle = m_pFileSystem->Open(pszFileLocation, "rb");
	if(!pFileHandle)
	{
		Warning("CSharedModelCache::Load( %s ): file not found\n", pszFileLocation);
		return Cache_Invalid;
	}

	// Get size and ensure we've got enough space
	int nSize = m_pFileSystem->Size(pFileHandle);
	node.sharedModelMemory.EnsureCapacity(nSize);

	// Try and read all the bytes out of the file
	bool bReadOk = (nSize == m_pFileSystem->Read(node.sharedModelMemory.Base(), nSize, pFileHandle));
	m_pFileSystem->Close(pFileHandle);

	// Check we read it OK
	if(!bReadOk)
	{
		Warning("CSharedModelCache::Load( %s ): unable to read %d bytes from file\n", pszFileLocation, nSize);
		return Cache_Invalid;
	}

	// Extract studiosharehdr_t from the memory
	node.pStudioShareHeader = (studiosharehdr_t*)node.sharedModelMemory.Base();
	
	// Check ID for "IDAG"
//	if(LittleLong(node.pStudioShareHeader->id) != 1195459657)
	if(LittleLong(node.pStudioShareHeader->id) != IDSTUDIOSHAREHEADER)
	{
		Warning("CSharedModelCache::Load( %s ): invalid ID, not a shared model file\n", pszFileLocation);
		return Cache_Invalid;
	}

	if(LittleLong(node.pStudioShareHeader->version) != STUDIO_VERSION)
	{
		Warning("CSharedModelCache::Load( %s ): invalid version (%d, expecting %d)\n", pszFileLocation, node.pStudioShareHeader->version, STUDIO_VERSION);
		return Cache_Invalid;
	}

	return nIndex+1;
}
