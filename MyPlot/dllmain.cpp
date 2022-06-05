#include "pch.h"

#include <stdexcept>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugString(L"[DllMain] dllmain.cpp|DllMain()|DLL_PROCESS_ATTACH\n");

		//DisableThreadLibraryCalls(hModule);

		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugString(L"[DllMain] dllmain.cpp|DllMain()|DLL_THREAD_ATTACH\n");
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString(L"[DllMain] dllmain.cpp|DllMain()|DLL_THREAD_DETACH\n");
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString(L"[DllMain] dllmain.cpp|DllMain()|DLL_PROCESS_DETACH\n");

		if (lpReserved != NULL)
			OutputDebugString(L"[DllMain] dllmain.cpp|DllMain()|DLL_PROCESS_DETACH|proc is terminating\n");

		break;
	}
	return TRUE;
}

