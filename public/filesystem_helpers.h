//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef FILESYSTEM_HELPERS_H
#define FILESYSTEM_HELPERS_H
#ifdef _WIN32
#pragma once
#endif
#include "filesystem.h" // FileHandle_t
#include "utlbuffer.h" // CUtlBuffer
//#include "utlvector.h" // CUtlVector
#include "characterset.h" // characterset_t


// Call until it returns NULL. Each time you call it, it will parse out a token.
const char* ParseFile( const char* pFileBytes, char* pToken, bool* pWasQuoted );
char* ParseFile( char* pFileBytes, char* pToken, bool* pWasQuoted );	// (same exact thing as the const version)

//-----------------------------------------------------------------------------
// Purpose: initializes all other FS_* functions for use
//-----------------------------------------------------------------------------
void FS_InitFilesystem(IBaseFileSystem *pFS);
void FS_InitFilesystem(IFileSystem *pFS);

//-----------------------------------------------------------------------------
// Purpose: read the contents of pszFile into buf
// Output : true = file was read into buf successfully
//			false = file wasn't read completely into buf
// Note: use Size() method to calculate buffer size, instead of TellPut()
// Note: buffer is NULL-terminated
//-----------------------------------------------------------------------------
bool FS_ReadFile(const char *pszFile, CUtlBuffer &buf, const char *pathID=NULL);


#endif // FILESYSTEM_HELPERS_H
