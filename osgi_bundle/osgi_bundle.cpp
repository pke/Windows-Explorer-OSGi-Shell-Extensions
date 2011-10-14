// osgi_bundle.cpp : Implementation of DLL Exports.
#include "stdafx.h"
#include "resource.h"
#include "osgi_bundle.h"

// Export entry points without the use of an external DEF file
#if defined (_M_IX86)
#pragma comment (linker, "/EXPORT:DllMain=_DllMain@12,PRIVATE")
#pragma comment (linker, \
  "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#pragma comment (linker, \
  "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")
#pragma comment (linker, \
  "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
#pragma comment (linker, \
  "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#elif defined (_M_IA64)
#pragma comment (linker, "/EXPORT:DllMain,PRIVATE")
#pragma comment (linker, "/EXPORT:DllRegisterServer,PRIVATE")
#pragma comment (linker, "/EXPORT:DllUnregisterServer,PRIVATE")
#pragma comment (linker, "/EXPORT:DllGetClassObject,PRIVATE")
#pragma comment (linker, "/EXPORT:DllCanUnloadNow,PRIVATE")
#endif

class Module : public ATL::CAtlDllModuleT<Module> {
public :
  Module() {
    //ATL::AtlEnableMemoryTracking(true);
  }

  ~Module() {
#ifdef _DEBUG
    ATL::AtlDumpMemoryLeaks();
#endif    
  }
}_AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD dwReason, LPVOID lpReserved) {
  return _AtlModule.DllMain(dwReason, lpReserved); 
}

STDAPI DllCanUnloadNow(void) {
  return _AtlModule.DllCanUnloadNow();
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
  return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer(void) {
  return _AtlModule.DllRegisterServer(false);
}

STDAPI DllUnregisterServer(void) {
	return _AtlModule.DllUnregisterServer(false);
}