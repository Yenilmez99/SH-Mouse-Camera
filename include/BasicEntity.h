#pragma once
#include <cstdint>

#pragma pack(1)
struct BasicEntity {
    float PosX = 0.0f;
    float PosY = 0.0f;
    float PosZ = 0.0f;
    int16_t Pitch = 0;
    int16_t BamsP = 0;
    int16_t Yaw = 0;
    int16_t BamsY = 0;
    int16_t Roll = 0;
    int16_t BamsR = 0;
};
#pragma pack()