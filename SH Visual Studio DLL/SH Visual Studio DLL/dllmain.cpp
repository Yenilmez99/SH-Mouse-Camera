#include "pch.h"
#include "AnaFonksiyon.h"
#include "TopluFonksiyon.h"

DWORD WINAPI AnaCekirdek(HMODULE hmodulu) {
    TopluFonksiyon();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hmodulu, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)AnaCekirdek, hmodulu, 0, nullptr));

    case DLL_PROCESS_DETACH:

    case DLL_THREAD_ATTACH:

    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
