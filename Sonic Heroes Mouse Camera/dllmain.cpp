#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <Windows.h>
#include <cmath>
#include <fstream>
#include <filesystem>

#include "json.hpp"

struct CameraSH {
    float_t PosX = 0.0f;
    float_t PosY = 0.0f;
    float_t PosZ = 0.0f;
    uint16_t Pitch = 0;
    uint16_t Unkown = 0;
    uint16_t Yaw = 0;
    uint16_t Unkown2 = 0;
    uint16_t Roll = 0;
};
struct BasicCharacterSH {
    float_t PosX;
    float_t PosY;
    float_t PosZ;
    uint16_t Pitch;
    uint16_t Unkown;
    uint16_t Yaw;
    uint16_t Unkown2;
    uint16_t Roll;
};
struct MouseInputSH {
    int32_t InputX;
    int32_t InputY;
};
struct ControllerInputSH {
    int32_t InputX;
    int32_t InputY;
};

inline std::filesystem::path GetReloaded2Path() {
    char* Reloaded2EnvVal = std::getenv("RELOADEDIIMODS");

    std::filesystem::path FullPath = "C:\\Reloaded-II\\User\\Mods\\sonicheroes.devicemod.mousecam\\Config.json";

    if (Reloaded2EnvVal != nullptr) {
        std::filesystem::path ReloadedModsPath = Reloaded2EnvVal;
        FullPath = ReloadedModsPath.parent_path() / "User\\Mods\\sonicheroes.devicemod.mousecam\\Config.json";
    }

    return FullPath;
}
inline void WriteNop(void* Address,size_t Size) {
    DWORD oldProtect;
    VirtualProtect(Address, Size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset(Address, 0x90, Size);
    VirtualProtect(Address, Size, oldProtect, &oldProtect);
}

DWORD WINAPI MainCore() {
    float_t SensivityX = 10.0f, SensivityY = 10.0f, Radius = 90.0f;

    std::filesystem::path Reloaded2Path = GetReloaded2Path();
    std::ifstream ConfigFile(Reloaded2Path);

    if (ConfigFile.is_open()) {
        try {
            nlohmann::json ConfigData;
            ConfigFile >> ConfigData;

            SensivityX = ConfigData.value("SensivityX", 10.0f);
            SensivityY = ConfigData.value("SensivityY", 10.0f);
        }
        catch (const nlohmann::json::parse_error&) {
        }

        ConfigFile.close();
    }

    CameraSH* myCamera = reinterpret_cast<CameraSH*>(0x00A60C30); // CameraX Address
    BasicCharacterSH* myCharacter = nullptr;
    MouseInputSH* myMouseInput = reinterpret_cast<MouseInputSH*>(0x00A2F930); // Mouse Input Table
    ControllerInputSH* myControllerInput = reinterpret_cast<ControllerInputSH*>(0x00A2FB14); // Controller Input Table

    int16_t TotalMouseInputX = 0, TotalMouseInputY = 0;
    uint8_t* GameState = reinterpret_cast<uint8_t*>(0x008D66F0); // Game State
    void** CharacterAddresses = reinterpret_cast<void**>(0x009CE820); // Character Address Pointer

    while (*CharacterAddresses == nullptr) Sleep(100);

    WriteNop(reinterpret_cast<void*>(0x006207DB), 2);
    WriteNop(reinterpret_cast<void*>(0x006207E3), 3);
    WriteNop(reinterpret_cast<void*>(0x006207EC), 3);
    WriteNop(reinterpret_cast<void*>(0x006207F8), 2);
    WriteNop(reinterpret_cast<void*>(0x00620800), 3);

    while (true) {

        if (!(*GameState == 5) || (*GameState == 4)) {
            Sleep(10);
            continue;
        }

        TotalMouseInputX += myMouseInput->InputX + myControllerInput->InputX;
        TotalMouseInputY += myMouseInput->InputY + myControllerInput->InputY;

        if (TotalMouseInputY * SensivityY < -16384) TotalMouseInputY = -16384 / SensivityY;
        if (TotalMouseInputY * SensivityY > 16384) TotalMouseInputY = 16384 / SensivityY;

        myCharacter = reinterpret_cast<BasicCharacterSH*>(reinterpret_cast<uintptr_t>(*CharacterAddresses) + 0xE8); // e8 is Character PosX Offset

        myCamera->PosX = myCharacter->PosX + Radius * cosf(TotalMouseInputX * SensivityX * M_PI / 32768)
                                                    * cosf(TotalMouseInputY * SensivityY * M_PI / 32768);
        myCamera->PosY = 8.5f + myCharacter->PosY + Radius * sinf(TotalMouseInputY * SensivityY * M_PI / 32768);

        myCamera->PosZ = myCharacter->PosZ + Radius * sinf(TotalMouseInputX * SensivityX * M_PI / 32768)
                                                    * cosf(TotalMouseInputY * SensivityY * M_PI / 32768);

        myCamera->Pitch = - TotalMouseInputY * SensivityY;
        myCamera->Yaw = - TotalMouseInputX * SensivityX + 16384;

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
    {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainCore, hModule, 0, nullptr);
        if (hThread != nullptr) CloseHandle(hThread);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}