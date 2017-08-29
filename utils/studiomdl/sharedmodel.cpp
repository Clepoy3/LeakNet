#include "engine/ISharedModelCache.h"

ISharedModelCache* g_pSharedModelCache = NULL;

void LoadSharedModelCacheInterface()
{
	CSysModule* pEngineModule = Sys_LoadModule("engine.dll");
	ErrorIfNot(pEngineModule, ("Unable to load \"engine.dll\", ensure it is in the same directory as \"studiomdl.exe\" and try again."));

	CreateInterfaceFn pfnFactory = Sys_GetFactory(pEngineModule);
	ErrorIfNot(pfnFactory, ("Unable to retrieve interface factory from \"engine.dll\"."));

	g_pSharedModelCache = (ISharedModelCache*)pfnFactory(SHARED_MODEL_CACHE_INTERFACE_VERSION, NULL);
	ErrorIfNot(g_pSharedModelCache, ("Unable to load shared model cache interface from \"engine.dll\"."));
}