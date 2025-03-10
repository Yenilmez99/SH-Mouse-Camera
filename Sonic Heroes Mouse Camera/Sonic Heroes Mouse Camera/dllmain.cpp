#include "pch.h"

#define PiNumber 3.1415
#define OnOffKey 0x4E // N key
#define ForceCloseKey 0x55 // U key
#define SensUpKey 0x72 // F3 Key
#define SensDownKey 0x73 // F4 Key
#define SensResetKey 0x74 // F5 Key
#define RadiusUpKey 0x75 // F6 Key
#define RadiusDownKey 0x76 // F7 Key
#define RadiusResetKey 0x77 // F8 Key

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

std::vector<uint8_t> AssemblyNopConverter(HANDLE HandleH, std::vector<int> Vectors, std::vector<uint8_t> OldHexValues) {
    uint8_t NopOP = 0x90;
    uint8_t OldHexValue = 0;
    for (uint8_t i = 0; i < Vectors.size(); i++) {
        ReadProcessMemory(HandleH, (PBYTE*)Vectors[i], &OldHexValue, sizeof(uint8_t), 0);
        OldHexValues.push_back(OldHexValue);
        WriteProcessMemory(HandleH, (PBYTE*)Vectors[i], &NopOP, sizeof(uint8_t), 0);
    }
    return OldHexValues;
}
void AssemblyNopRepair(HANDLE HandleH, std::vector<int> Vectors, std::vector<uint8_t> RepairHexValue) {
    for (short i = 0; i < Vectors.size(); i++) {
        WriteProcessMemory(HandleH, (PBYTE*)Vectors[i], &RepairHexValue[i], sizeof(uint8_t), 0);
    }
}

DWORD WINAPI MainCore(HMODULE hModule) {

    bool StageOpen = 1, OnOff = 1, GamePause = 0;

    short int CameraRotation[2] = { 0,0 };
    int MousePosDifference[2] = { 0,0 };

    float CharacterPos[3] = { 0.0f,0.0f,0.0f }, CameraPosition[3] = { 0.0f,0.0f,0.0f };
    float Radius = 90.0f;

    std::vector<int> Addresses = { 0x006207DB,0x006207DC ,0x006207E3 ,0x006207E4 ,0x006207E5 ,0x006207EC ,
                                   0x006207ED ,0x006207EE ,0x00620809 ,0x0062080A ,0x0062080B ,0x00620800 ,
                                   0x00620801 ,0x00620802 ,0x006207F8 ,0x006207F9 };
    std::vector<uint8_t> OldHexValues;

    POINT MousePosition;
    XINPUT_STATE state;
    DWORD dwResult;
    SHORT rightThumbX;
    SHORT rightThumbY;

    HWND hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)");

    while (hwnd_SonicHeroesTM == NULL) {
        hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)");
        Sleep(100);
    }

    DWORD ProcessIDSonicHeroes = NULL;
    GetWindowThreadProcessId(hwnd_SonicHeroesTM, &ProcessIDSonicHeroes);
    HANDLE HandleSonicHeroes = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessIDSonicHeroes);
    char SonicHeroesGameModule[] = "Tsonic_win.exe";
    DWORD SonicHeroesMainAdress = GetModuleBaseAddress(_T(SonicHeroesGameModule), ProcessIDSonicHeroes);

    DWORD CharacterPosMainAdres = 0x005CE820;

    std::vector<DWORD> CharacterPosOffset{ 0xE8 };

    DWORD CharacterPosAdres = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesMainAdress, CharacterPosMainAdres, CharacterPosOffset);

    ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x007C6BD4, &StageOpen, sizeof(bool), 0);
    ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6708, &GamePause, sizeof(bool), 0);

    SetCursorPos(800, 600);

    std::ifstream File("Y99 Mods Config File.txt");
    std::string Line;
    float Sensivity = 10.0f;
    if (!File.is_open())
        Sensivity = 10.0f;
    else {
        for (int i = 0; i < 7; i++) {
            std::getline(File, Line);
            if (i == 6) {
                std::stringstream ss(Line);
                std::string Key;
                ss >> Key >> Sensivity;
            }
        }
        if (Sensivity <= 1000.0f && Sensivity > 0.0f) {

        }
        else
            Sensivity = 10.0f;
    }
    File.close();

    // Camera Freeze
    OldHexValues = AssemblyNopConverter(HandleSonicHeroes, Addresses, OldHexValues);

    while (hwnd_SonicHeroesTM != 0) {

        // Stage and Window ON
        if (!StageOpen && !GamePause && OnOff && hwnd_SonicHeroesTM == GetForegroundWindow()) {
            GetCursorPos(&MousePosition);
            MousePosDifference[0] += MousePosition.x - 800;
            MousePosDifference[1] += MousePosition.y - 600;
            SetCursorPos(800, 600);

            if (MousePosDifference[0] * Sensivity > 65536)
                MousePosDifference[0] = (MousePosDifference[0] % 65536) / Sensivity;

            if (MousePosDifference[1] * Sensivity > 16384)
                MousePosDifference[1] = 16384 / Sensivity;

            if (MousePosDifference[1] * Sensivity < -16384)
                MousePosDifference[1] = -16384 / Sensivity;

            dwResult = XInputGetState(0, &state);

            if (dwResult == ERROR_SUCCESS) {
                rightThumbX = state.Gamepad.sThumbRX;
                rightThumbY = state.Gamepad.sThumbRY;

                if (rightThumbX > 100 && rightThumbX < 8192)
                    MousePosDifference[0] += 10;
                if (rightThumbX >= 8192 && rightThumbX < 16384)
                    MousePosDifference[0] += 20;
                if (rightThumbX >= 16384 && rightThumbX < 24576)
                    MousePosDifference[0] += 35;
                if (rightThumbX >= 24576 && rightThumbX < 32768)
                    MousePosDifference[0] += 50;

                if (rightThumbX < -100 && rightThumbX > -8192)
                    MousePosDifference[0] -= 10;
                if (rightThumbX <= -8192 && rightThumbX > -16384)
                    MousePosDifference[0] -= 20;
                if (rightThumbX <= -16384 && rightThumbX > -24576)
                    MousePosDifference[0] -= 35;
                if (rightThumbX <= -24576 && rightThumbX >= -32768)
                    MousePosDifference[0] -= 50;

                if (rightThumbY > 100 && rightThumbY < 8192)
                    MousePosDifference[1] -= 10;
                if (rightThumbY >= 8192 && rightThumbY < 16384)
                    MousePosDifference[1] -= 20;
                if (rightThumbY >= 16384 && rightThumbY < 24576)
                    MousePosDifference[1] -= 35;
                if (rightThumbY >= 24576 && rightThumbY < 32768)
                    MousePosDifference[1] -= 50;

                if (rightThumbY < -100 && rightThumbY > -8192)
                    MousePosDifference[1] += 10;
                if (rightThumbY <= -8192 && rightThumbY > -16384)
                    MousePosDifference[1] += 20;
                if (rightThumbY <= -16384 && rightThumbY > -24576)
                    MousePosDifference[1] += 35;
                if (rightThumbY <= -24576 && rightThumbY >= -32768)
                    MousePosDifference[1] += 50;
            }

            CharacterPosAdres = GetPointerAddress(hwnd_SonicHeroesTM, SonicHeroesMainAdress, CharacterPosMainAdres, CharacterPosOffset);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)CharacterPosAdres, &CharacterPos[0], sizeof(float), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(CharacterPosAdres + 0x4), &CharacterPos[1], sizeof(float), 0);
            ReadProcessMemory(HandleSonicHeroes, (PBYTE*)(CharacterPosAdres + 0x8), &CharacterPos[2], sizeof(float), 0);

            CameraPosition[0] = CharacterPos[0] + Radius * cos(MousePosDifference[1] * Sensivity * PiNumber / 32768) * cos(MousePosDifference[0] * Sensivity * PiNumber / 32768);
            CameraPosition[1] = 8.5f + CharacterPos[1] + Radius * sin(MousePosDifference[1] * Sensivity * PiNumber / 32768);
            CameraPosition[2] = CharacterPos[2] + Radius * cos(MousePosDifference[1] * Sensivity * PiNumber / 32768) * sin(MousePosDifference[0] * Sensivity * PiNumber / 32768);
            
            CameraRotation[0] = -Sensivity * MousePosDifference[0] + 16384;
            CameraRotation[1] = -Sensivity * MousePosDifference[1];

            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C30, &CameraPosition[0], sizeof(float), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C34, &CameraPosition[1], sizeof(float), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C38, &CameraPosition[2], sizeof(float), 0);

            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C40, &CameraRotation[0], sizeof(short int), 0);
            WriteProcessMemory(HandleSonicHeroes, (PBYTE*)0x00A60C3C, &CameraRotation[1], sizeof(short int), 0);
        }

        // Stage and Window OFF
        if (GetAsyncKeyState(OnOffKey) & 1) {
            OnOff = !OnOff;
            if (OnOff)
                OldHexValues = AssemblyNopConverter(HandleSonicHeroes, Addresses, OldHexValues);
            if (!OnOff)
                AssemblyNopRepair(HandleSonicHeroes, Addresses, OldHexValues);
        }

        if (GetAsyncKeyState(ForceCloseKey) & 1)
            break;

        if (GetAsyncKeyState(SensUpKey) & 1)
            Sensivity += 1;
        if (GetAsyncKeyState(SensDownKey) & 1)
            if (Sensivity > 0)
                Sensivity -= 1;
        if (GetAsyncKeyState(SensResetKey) & 1)
            Sensivity = 10.0f;
        if (GetAsyncKeyState(RadiusUpKey) & 1)
            Radius += 5;
        if (GetAsyncKeyState(RadiusDownKey) & 1)
            if (Radius > 0)
                Radius -= 5;
        if (GetAsyncKeyState(RadiusResetKey) & 1)
            Radius = 90.0f;

        // Controls
        Sleep(16);
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x007C6BD4, &StageOpen, sizeof(bool), 0);
        ReadProcessMemory(HandleSonicHeroes, (PBYTE*)0x008D6708, &GamePause, sizeof(bool), 0);
        hwnd_SonicHeroesTM = FindWindowA(NULL, "SONIC HEROES(TM)");
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

