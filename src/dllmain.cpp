#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <numbers>
#include <wtypesbase.h>

#include "helper.h"
#include "MinHook.h"

// There are these for hide cursor
typedef HCURSOR(WINAPI* PSETCURSOR)(HCURSOR);
typedef int(WINAPI* PSHOWCURSOR)(BOOL);
PSETCURSOR fpSetCursor = NULL;                  PSHOWCURSOR fpShowCursor = NULL;
// Hook SetCursor
HCURSOR WINAPI DetourSetCursor(HCURSOR hCursor) {return fpSetCursor(NULL);}
// Hook ShowCursor
int WINAPI DetourShowCursor(BOOL bShow)         {return fpShowCursor(FALSE);}
// Orginal Camera Function
typedef void(__cdecl* OrginalUpdateCamera)();
OrginalUpdateCamera fpCameraUpdate = nullptr;

inline Settings ConfigSettings;
volatile uint8_t& GameState = *reinterpret_cast<volatile uint8_t*>(0x008D66F0);
static signed int Total[2] = {};
bool TurnOrginalCamera = false;
void __cdecl HookUpdateCamera() {
    if (TurnOrginalCamera) return fpCameraUpdate();
    
    if (GameState != 4 && GameState != 5) return fpCameraUpdate();

    volatile uintptr_t& RoleBase = *reinterpret_cast<volatile uintptr_t*>(0x00A4C268);
    volatile uint8_t& Role = *reinterpret_cast<uint8_t*>(RoleBase + 0x3B);

    volatile uintptr_t& CharacterBase = reinterpret_cast<volatile uintptr_t*>(0x00A4B1B0)[Role];
    volatile BasicEntity& Character = *reinterpret_cast<volatile BasicEntity*>(CharacterBase + 0x114);

    volatile BasicInput& Mouse = *reinterpret_cast<volatile BasicInput*>(0x00A2F930);
    volatile BasicInput& Controller = *reinterpret_cast<volatile BasicInput*>(0x00A2FB14);
    Total[0] += ConfigSettings.InvertXAxis ? -(Mouse.x + Controller.x) : Mouse.x + Controller.x;
    Total[1] += ConfigSettings.InvertYAxis ? -(Mouse.y + Controller.y) : Mouse.y + Controller.y;
    
    const signed int MaxY = static_cast<signed int>(16384.0f / ConfigSettings.Sensitivity);

    if (Total[1] < -MaxY) Total[1] = -MaxY;
    if (Total[1] > MaxY) Total[1] = MaxY;

    const float TargetCamX = Total[0] * ConfigSettings.Sensitivity;
    const float TargetCamY = Total[1] * ConfigSettings.Sensitivity;

    static float CurrentCamX = 0.0f;
    static float CurrentCamY = 0.0f;

    CurrentCamX += (TargetCamX - CurrentCamX) * ConfigSettings.CameraRotationSmoothFactor;
    CurrentCamY += (TargetCamY - CurrentCamY) * ConfigSettings.CameraRotationSmoothFactor;

    const float RAD_MULT = static_cast<float>(std::numbers::pi / 32768.0f);
    const float RadX = CurrentCamX * RAD_MULT, RadY = CurrentCamY * RAD_MULT;

    const float TargetCamPosX = Character.Pos[0] + ConfigSettings.Radius * cosf(RadX) * cosf(RadY);
    const float TargetCamPosY = 8.5f + Character.Pos[1] + ConfigSettings.Radius * sinf(RadY);
    const float TargetCamPosZ = Character.Pos[2] + ConfigSettings.Radius * sinf(RadX) * cosf(RadY);

    volatile BasicEntity& Camera = *reinterpret_cast<volatile BasicEntity*>(0x00A60C30);
    Camera.Pos[0] += (TargetCamPosX - Camera.Pos[0]) * ConfigSettings.CameraPositionSmoothFactor;
    Camera.Pos[1] += (TargetCamPosY - Camera.Pos[1]) * ConfigSettings.CameraPositionSmoothFactor;
    Camera.Pos[2] += (TargetCamPosZ - Camera.Pos[2]) * ConfigSettings.CameraPositionSmoothFactor;

    Camera.Rot[0] = static_cast<short>(-CurrentCamY);
    Camera.Rot[1] = static_cast<short>(-CurrentCamX) + 16384;

    return;
}

DWORD WINAPI MainCore(LPVOID lpParam) {
    HMODULE myHModule = reinterpret_cast<HMODULE>(lpParam);

    GetSettings(myHModule, ConfigSettings);

    MH_STATUS mhStatus = MH_Initialize();
    if (mhStatus != MH_OK && mhStatus != MH_ERROR_ALREADY_INITIALIZED) {
        return 0;
    }

    if (ConfigSettings.HideCursor) {
        MH_CreateHook(reinterpret_cast<LPVOID>(&SetCursor), reinterpret_cast<LPVOID>(&DetourSetCursor), reinterpret_cast<LPVOID*>(&fpSetCursor));
        MH_CreateHook(reinterpret_cast<LPVOID>(&ShowCursor), reinterpret_cast<LPVOID>(&DetourShowCursor), reinterpret_cast<LPVOID*>(&fpShowCursor));
    }
    MH_CreateHook(reinterpret_cast<LPVOID>(0x006207A0), reinterpret_cast<LPVOID>(&HookUpdateCamera), reinterpret_cast<LPVOID*>(&fpCameraUpdate));

    // Active to Hooks
    MH_EnableHook(MH_ALL_HOOKS);

    // Stuck Loop
    while (!(GetAsyncKeyState(VK_F1)&0x1)) {
        if (GetAsyncKeyState('N') & 0x1) TurnOrginalCamera = !TurnOrginalCamera;
        if (ConfigSettings.CursorLock) {
            if (GameState == 4 || GameState == 5) SetCursorPos(400, 300);
            Sleep(16);
        }
        else Sleep(100);
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH:
    {
        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainCore, hModule, 0, nullptr);
        if (hThread != nullptr) CloseHandle(hThread);
        break;
    }
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}