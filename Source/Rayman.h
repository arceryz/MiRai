#ifndef RAYMAN_H
#define RAYMAN_H

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
using namespace std;

#define ZERO { 0, 0, 0 }
#define ONE { 1, 1, 1 }
#define UNIT_X { 1, 0, 0 }
#define UNIT_Y { 0, 1, 0 }
#define UNIT_Z { 0, 0, 1 }

float Sign(float x);

void DragCameraUpdate(Camera2D &camera, float &zoomTarget, bool &centeredZoom, float zoomMin=0.1f, float zoomMax=2.0f, float zoomSpeed=0.12f, float lerpSpeed=1, float moveSpeed=0);

#endif