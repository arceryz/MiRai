#ifndef CAMERA_H
#define CAMERA_H

#include <raylib.h>
#include "Rayman.h"

class XCamera {
public:
    Camera3D internal;
    float fovy = 45.0f;
    float orthoSize = 2.0f;
    bool perspective = true;

    Vector3 forward = UNIT_Z;
    Vector3 left = UNIT_X;

    XCamera();
    ~XCamera();
    void Begin();
    void RotateH(float deg);
    void RotateV(float deg);
    void SetRadius(float rad);
    float GetRadius();
};

#endif