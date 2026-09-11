#pragma once
#include <fstream>
#include <windows.h>
#include <filesystem>

#include "nlohmann/json.hpp"

struct Settings {
    float       Sensitivity = 10.0f;
    float       Radius = 70.0f;
    float       CameraPositionSmoothFactor = 1.0f; // 1: No Smooth
    float       CameraRotationSmoothFactor = 1.0f; // 1: No Smooth
    bool        HideCursor = false;
    bool        CursorLock = false;
    bool        InvertXAxis = false;
    bool        InvertYAxis = false;
};

struct BasicEntity {
    float       Pos[3] = {};
    signed int  Rot[3] = {};
};

struct BasicInput {
    signed int  x = 0;
    signed int  y = 0;
};

inline void GetSettings(HMODULE& dllHModule, Settings& settings) {
    char dllPathStr[MAX_PATH];
    
    // get dlls full path (example: C:\Mods\MyMod\mydll.dll)
    GetModuleFileNameA(dllHModule, dllPathStr, MAX_PATH);

    // change it and find config files path
    std::filesystem::path DllPath(dllPathStr);
    std::filesystem::path ConfigFilePath = DllPath.parent_path().parent_path().parent_path() / "User/Mods/sonicheroes.devicemod.mousecam/Config.json";

    std::ifstream ConfigFile(ConfigFilePath);

    if (ConfigFile.is_open()) {
        try {
            nlohmann::json ConfigData;
            ConfigFile >> ConfigData;

            settings.Sensitivity = ConfigData.value("Sensitivity", 10.0f);
            settings.Radius = ConfigData.value("Radius", 70.0f);
            settings.CameraPositionSmoothFactor = ConfigData.value("CameraPositionSmoothFactor", 1.0f);
            settings.CameraRotationSmoothFactor = ConfigData.value("CameraRotationSmoothFactor", 1.0f);
            settings.HideCursor = ConfigData.value("HideCursor", false);
            settings.CursorLock = ConfigData.value("CursorLock", false);
            settings.InvertXAxis = ConfigData.value("InvertXAxis", false);
            settings.InvertYAxis = ConfigData.value("InvertYAxis", false);

        }
        catch (const nlohmann::json::exception&) {
        }

        ConfigFile.close();
    }
}