#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <string>

#include "framework.h"

DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}
DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, std::vector<DWORD> offsets) {
    DWORD pID = NULL;
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    DWORD offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null;
    for (int i = 0; i < offsets.size() - 1; i++)
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1);
}

DWORD WINAPI MainCore(HMODULE hModule) {
    bool RunStage = 0, GamePause = 0, CameraFreeze = 0, CameraUnFreeze = 1;
    short int CameraRotationV = 0, CameraRotationH = 0;
    int MousePosXDelta = 0, MousePosYDelta = 0;
    const float SensivityV = 0.2f, SensivityH = 0.3f;
    const float Radius = 140.0f;
    float CharacterXPos = 0.0f, CharacterYPos = 0.0f, CharacterZPos = 0.0f;
    float CameraXPos = 0.0f, CameraYPos = 0.0f, CameraZPos = 0.0f;
    float DegreeAlpha = 0.0f, DegreeBeta = 0.0f;

    POINT MousePos;

    HWND hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)");

    while (hwnd_SonicHeroesTM == NULL) {
        hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)");
    }

    DWORD ProcessIDSonicHeroes = NULL;
    GetWindowThreadProcessId(hwnd_SonicHeroesTM, &ProcessIDSonicHeroes);
    HANDLE HandleSonicHeroes = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessIDSonicHeroes);
    char SonicHeroesGameModule[] = "Tsonic_win.exe";
    DWORD SonicHeroesMainAdress = GetModuleBaseAddress(_T(SonicHeroesGameModule), ProcessIDSonicHeroes);

    DWORD CharacterXPosMainAdres = 0x005CE820;

    std::vector<DWORD> CharacterXPosOffset{ 0xE8 };

    DWORD UseCharacterXPosAdres;
    DWORD UseCameraFreezeAdres;

    while (true) {
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x007C6BD4, &RunStage, sizeof(bool), 0);
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6708, &GamePause, sizeof(bool), 0);

        if (RunStage == 0 && GamePause == 0 && GetAsyncKeyState(0x4E) == 0){
            UseCharacterXPosAdres = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesMainAdress, CharacterXPosMainAdres, CharacterXPosOffset);

            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)UseCharacterXPosAdres, &CharacterXPos, sizeof(float), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseCharacterXPosAdres + 0x4), &CharacterYPos, sizeof(float), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(UseCharacterXPosAdres + 0x8), &CharacterZPos, sizeof(float), 0);

            GetCursorPos(&MousePos);
            SetCursorPos(800, 600);

            MousePosXDelta += 800 - MousePos.x;
            MousePosYDelta += MousePos.y - 600;

            if (MousePosYDelta * SensivityV > 89) {
                MousePosYDelta = 89 / SensivityV;
            }
            else if (MousePosYDelta * SensivityV < -89) {
                MousePosYDelta = -89 / SensivityV;
            }
            else{
            }

            CameraXPos = CharacterXPos + Radius * cos(SensivityV * MousePosYDelta * 3.14f / 180) * sin(SensivityH * MousePosXDelta * 3.14f / 180);
            CameraYPos = CharacterYPos + Radius * sin(SensivityV * MousePosYDelta * 3.14f / 180);
            CameraZPos = CharacterZPos + Radius * cos(SensivityV * MousePosYDelta * 3.14f / 180) * cos(SensivityH * MousePosXDelta * 3.14f / 180);
            CameraRotationH = SensivityH * MousePosXDelta * 182;
            CameraRotationV = - SensivityV * MousePosYDelta * 182;

            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A69880, &CameraFreeze, sizeof(bool), 0); // Freeze Camera
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C30, &CameraXPos, sizeof(float), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C34, &CameraYPos, sizeof(float), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C38, &CameraZPos, sizeof(float), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C40, &CameraRotationH, sizeof(short int), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C3C, &CameraRotationV, sizeof(short int), 0);
        }
        else{
        }

        if (GetAsyncKeyState(0x55)) {
            break;
        }
        else if (GetAsyncKeyState(0x4E) != 0) {
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A69880, &CameraUnFreeze, sizeof(bool), 0);
        }
        else {
        }

        Sleep(10);

    }

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainCore, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

