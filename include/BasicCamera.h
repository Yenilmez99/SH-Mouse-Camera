#pragma once
#include <cstdint>

struct BasicCamera {
    float PosX = 0.0f;
    float PosY = 0.0f;
    float PosZ = 0.0f;
    uint16_t Pitch = 0;
    uint16_t Unkown = 0;
    uint16_t Yaw = 0;
    uint16_t Unkown2 = 0;
    uint16_t Roll = 0;
};