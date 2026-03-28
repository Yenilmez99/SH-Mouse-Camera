#include <windows.h>
#include <MinHook.h>
#include <nlohmann/json.hpp>
#include <cmath>
#include <fstream>
#include <filesystem>

#include "BasicCamera.h"
#include "BasicCharacter.h"
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
OrginalUpdateCamera fpCameraUpdate = NULL;

int64_t TotalInputX = 0, TotalInputY = 0;
uint32_t CharacterPtr = 0x009CE820;
volatile float Sensitivity = 10.0f;
volatile float Radius = 70.0f;
// Hook Camera Function
void __cdecl HookUpdateCamera() {

    if (*reinterpret_cast<volatile void**>(CharacterPtr) == nullptr) return;

    volatile uint8_t* GameState = reinterpret_cast<volatile uint8_t*>(0x008D66F0);
    if (*GameState != 5) {
        Sleep(10);
        return;
    }

    volatile BasicInput* MouseInput = reinterpret_cast<volatile BasicInput*>(0x00A2F930);
    volatile BasicInput* ControllerInput = reinterpret_cast<volatile BasicInput*>(0x00A2FB14);

    TotalInputX += MouseInput->InputX + ControllerInput->InputX;
    TotalInputY += MouseInput->InputY + ControllerInput->InputY;

    if (TotalInputY * Sensitivity < -16384) TotalInputY = -16384 / Sensitivity;
    if (TotalInputY * Sensitivity > 16384) TotalInputY = 16384 / Sensitivity;

    volatile BasicCharacter* myCharacter = reinterpret_cast<volatile BasicCharacter*>(*reinterpret_cast<volatile uint32_t*>(CharacterPtr) + 0xE8);
    volatile BasicCamera* myCamera = reinterpret_cast<volatile BasicCamera*>(0x00A60C30);

    myCamera->PosX = myCharacter->PosX + Radius * cosf(TotalInputX * Sensitivity * M_PI / 32768) * cosf(TotalInputY * Sensitivity * M_PI / 32768);
    myCamera->PosY = 8.5f + myCharacter->PosY + Radius * sinf(TotalInputY * Sensitivity * M_PI / 32768);
    myCamera->PosZ = myCharacter->PosZ + Radius * sinf(TotalInputX * Sensitivity * M_PI / 32768) * cosf(TotalInputY * Sensitivity * M_PI / 32768);

    myCamera->Pitch = - TotalInputY * Sensitivity;
    myCamera->Yaw = - TotalInputX * Sensitivity + 16384;

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

    if (MH_Initialize() != MH_OK) {
        FreeLibraryAndExitThread(myHModule, 0);
        return 1;
    }

    // Create Hook to SetCursor and ShowCursor Function
    MH_CreateHook(reinterpret_cast<LPVOID>(&SetCursor), reinterpret_cast<LPVOID>(&DetourSetCursor), reinterpret_cast<LPVOID*>(&fpSetCursor));
    MH_CreateHook(reinterpret_cast<LPVOID>(&ShowCursor), reinterpret_cast<LPVOID>(&DetourShowCursor), reinterpret_cast<LPVOID*>(&fpShowCursor));
    MH_CreateHook(reinterpret_cast<LPVOID>(0x006207A0), reinterpret_cast<LPVOID*>(HookUpdateCamera), reinterpret_cast<LPVOID*>(&fpCameraUpdate));

    // Do Active to Hooks
    bool HasAnyError = false;
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) HasAnyError = true;

    // Stuck Loop
    while (!HasAnyError && !(GetAsyncKeyState(VK_F1)&0x1)) {
        Sleep(100);
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    
    FreeLibraryAndExitThread(myHModule, 0);
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
