//======= Copyright © 2008-2011, The Axel Project, all rights reserved ======//
//
// Author: Saul Rennison <saul.rennison@gmail.com>
// Purpose: Shared model cache file, used by bone_setup.cpp 
//
//===========================================================================//

#ifndef ISHAREDMODELCACHE_H
#define ISHAREDMODELCACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"
#include "studio.h"
#include "filesystem.h"

//-----------------------------------------------------------------------------
// Purpose: holds pre-loaded ISharedModel instances so every time a shared
//          model animation wishes to be accessed, it doesn't have to be loaded
//          again
//-----------------------------------------------------------------------------
#define SHARED_MODEL_CACHE_INTERFACE_VERSION "ISharedModelCache004"

class ISharedModelCache : public IBaseInterface
{
public:
	//-------------------------------------------------------------------------
	// Call this when your DLL / EXE initializes with the filesystem you wish
	// to use
	//-------------------------------------------------------------------------
	virtual void InitFileSystem(IBaseFileSystem* pFileSystem) = 0;

	//-------------------------------------------------------------------------
	// Gets the studiosharehdr_t pointer relating to the shared model index
	//
	// NOTE: This function will return NULL if the index is invalid (i.e. not
	//       in the cache)
	//
	// VERSION 4 UPDATE: This function no longer takes an unsigned int as a
	//                   parameter. The cache supports 2,147,483,647 shared
	//                   models in it's cache
	//-------------------------------------------------------------------------
	virtual studiosharehdr_t* GetSharedModel(int nIndex) = 0;

	//-------------------------------------------------------------------------
	// Loads a shared model, has two return values:
	// -> Cache_Invalid (-1): NULL file location OR file not found OR not a
	//    shared model file OR unable to read all bytes from file
	//
	// -> Value larger than 0: file was loaded correctly, the return value is
	//    index
	//
	// NOTE: This function will Error() if InitFileSystem() is not called, or
	//       the pointer to the filesystem provided is NULL
	//-------------------------------------------------------------------------
	virtual int Load(const char* pszFileLocation) = 0;
};

extern ISharedModelCache* g_pSharedModelCache;

#endif
