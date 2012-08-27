//======= Copyright © 2008-2011, The Axel Project, all rights reserved ======//
//
// Author: Saul Rennison <saul.rennison@gmail.com>
// Purpose: Shared model cache file, used by bone_setup.cpp
//
//===========================================================================//

#ifndef CSHAREDMODELCACHE_H
#define CSHAREDMODELCACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "engine/ISharedModelCache.h"
#include "utllinkedlist.h"
#include "studio.h"

//-----------------------------------------------------------------------------
// Cache linked list node
//-----------------------------------------------------------------------------
struct SharedModelCacheNode_s
{
	CUtlMemory<byte> sharedModelMemory;
	studiosharehdr_t* pStudioShareHeader;
};

//-----------------------------------------------------------------------------
// Purpose: holds pre-loaded ISharedModel instances so every time a shared
//          model animation wishes to be accessed, it doesn't have to be loaded
//          again
//-----------------------------------------------------------------------------
class CSharedModelCache : public ISharedModelCache
{
public:
	CSharedModelCache() 
	{
		m_bInitted = false;
		m_pFileSystem = NULL;
	};

	virtual ~CSharedModelCache() {};

public:
	virtual void InitFileSystem(IBaseFileSystem* pFileSystem);
	virtual studiosharehdr_t* GetSharedModel(int nIndex);
	virtual int Load(const char* pszFileLocation);

private:
	bool m_bInitted;
	IBaseFileSystem* m_pFileSystem;
	CUtlLinkedList<SharedModelCacheNode_s, int> m_CacheNodes;
};

#endif
