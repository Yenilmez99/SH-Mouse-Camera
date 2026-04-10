#define _USE_MATH_DEFINES

#include <windows.h>
#include <MinHook.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <cmath>
#include <fstream>
#include <filesystem>

#include "BasicEntity.h"
#include "BasicInput.h"

// There are these for hide cursor
typedef HCURSOR(WINAPI* PSETCURSOR)(HCURSOR);
typedef int(WINAPI* PSHOWCURSOR)(BOOL);
PSETCURSOR fpSetCursor = NULL;
PSHOWCURSOR fpShowCursor = NULL;

// Hooks
// Hook SetCursor
HCURSOR WINAPI DetourSetCursor(HCURSOR hCursor) { return fpSetCursor(NULL);
}
// Hook ShowCursor
int WINAPI DetourShowCursor(BOOL bShow) {return fpShowCursor(FALSE);
}

// Orginal Camera Function
typedef void(__cdecl* OrginalUpdateCamera)();
OrginalUpdateCamera fpCameraUpdate = nullptr;

// Helpers
uintptr_t SonicHeroesBaseAddress = 0x400000;    volatile uintptr_t* RolePtr = nullptr;
volatile uintptr_t* CharacterPtr = nullptr;     volatile uint8_t* GameState = nullptr;
volatile float Sensitivity = 10.0f;             volatile float Radius = 70.0f;
int64_t TotalInputX = 0,                        TotalInputY = 0;

// Variables
volatile BasicEntity* Camera = nullptr;     volatile BasicEntity* Character = nullptr;
volatile BasicInput* MouseInput = nullptr;  volatile BasicInput* ControllerInput = nullptr;
volatile uint8_t* CameraState = nullptr;    volatile uint8_t* Role = nullptr;
volatile uint8_t useRole = 0;
// Hook Camera Function
void __cdecl HookUpdateCamera() {
    if (*CharacterPtr == 0x0 || *CameraState == 13 || *CameraState == 14) return fpCameraUpdate();
    if (*GameState != 4 && *GameState != 5) return fpCameraUpdate();
    Role = reinterpret_cast<volatile uint8_t*>(*RolePtr + 0x3B);
    useRole = *Role;
    if (*Role > 2) useRole = 0;
    Character = reinterpret_cast<volatile BasicEntity*>(CharacterPtr[useRole] + 0x114);

    TotalInputX += MouseInput->InputX + ControllerInput->InputX;
    TotalInputY += MouseInput->InputY + ControllerInput->InputY;

    int64_t MaxY = static_cast<int64_t>(16384.0f / Sensitivity);

    if (TotalInputY < -MaxY) TotalInputY = -MaxY;
    if (TotalInputY > MaxY) TotalInputY = MaxY;

    float OptX = TotalInputX * Sensitivity, OptY = TotalInputY * Sensitivity;
    const float RAD_MULT = static_cast<float>(M_PI / 32768.0f);
    float RadX = OptX * RAD_MULT,           RadY = OptY * RAD_MULT;

    Camera->PosX = Character->PosX + Radius * cosf(RadX) * cosf(RadY);
    Camera->PosY = 8.5f + Character->PosY + Radius * sinf(RadY);
    Camera->PosZ = Character->PosZ + Radius * sinf(RadX) * cosf(RadY);

    Camera->Pitch = - OptY;
    Camera->Yaw = static_cast<int32_t>(-OptX) + 16384;

    return;
}

DWORD WINAPI MainCore(LPVOID lpParam) {
    HMODULE myHModule = reinterpret_cast<HMODULE>(lpParam);

    char* Reloaded2EnvVal = std::getenv("RELOADEDIIMODS");
    std::filesystem::path Reloaded2Path = "C:/Reloaded-II";
    if (Reloaded2EnvVal != nullptr) Reloaded2Path = Reloaded2EnvVal;

    std::ifstream ConfigFile(Reloaded2Path.parent_path() / "User/Mods/sonicheroes.devicemod.mousecam/Config.json");
    if (ConfigFile.is_open()) {
        try {
            nlohmann::json ConfigData;
            ConfigFile >> ConfigData;

            Sensitivity = ConfigData.value("Sensitivity", 10.0f);
            Radius = ConfigData.value("Radius", 70.0f);

            if (Sensitivity < 0.01f) Sensitivity = 0.01f;
            if (Radius < 10.0f) Radius = 70.0f;

        }
        catch (const nlohmann::json::parse_error&) {
        }

        ConfigFile.close();
    }

    MH_STATUS mhStatus = MH_Initialize();
    if (mhStatus != MH_STATUS::MH_OK && mhStatus != MH_STATUS::MH_ERROR_ALREADY_INITIALIZED) {
        return 1;
    }

    SonicHeroesBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
    RolePtr = reinterpret_cast<volatile uintptr_t*>(0x0064C268 + SonicHeroesBaseAddress);
    CharacterPtr = reinterpret_cast<volatile uintptr_t*>(0x0064B1B0 + SonicHeroesBaseAddress);
    GameState = reinterpret_cast<volatile uint8_t*>(0x004D66F0 + SonicHeroesBaseAddress);

    Camera = reinterpret_cast<volatile BasicEntity*>(0x00660C30 + SonicHeroesBaseAddress);
    CameraState = reinterpret_cast<volatile uint8_t*>(0x00660C7C + SonicHeroesBaseAddress);
    MouseInput = reinterpret_cast<volatile BasicInput*>(0x0062F930 + SonicHeroesBaseAddress);
    ControllerInput = reinterpret_cast<volatile BasicInput*>(0x0062FB14 + SonicHeroesBaseAddress);

    // Create Hook to SetCursor and ShowCursor Function
    MH_CreateHook(reinterpret_cast<LPVOID>(&SetCursor), reinterpret_cast<LPVOID>(&DetourSetCursor), reinterpret_cast<LPVOID*>(&fpSetCursor));
    MH_CreateHook(reinterpret_cast<LPVOID>(&ShowCursor), reinterpret_cast<LPVOID>(&DetourShowCursor), reinterpret_cast<LPVOID*>(&fpShowCursor));
    MH_CreateHook(reinterpret_cast<LPVOID>(0x002207A0 + SonicHeroesBaseAddress), reinterpret_cast<LPVOID>(&HookUpdateCamera), reinterpret_cast<LPVOID*>(&fpCameraUpdate));

    // Do Active to Hooks
    MH_EnableHook(MH_ALL_HOOKS);

    // Stuck Loop
    while (!(GetAsyncKeyState(VK_F1)&0x1)) {
        Sleep(100);
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    
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
            DisableThreadLibraryCalls(hModule);
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
