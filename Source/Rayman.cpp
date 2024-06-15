#include "Rayman.h"


float Sign(float x)
{
    return (x == 0) ? 0: (x > 0 ? 1: -1);
}

void DragCameraUpdate(Camera2D &camera, float &zoomTarget, bool &centeredZoom, float zoomMin, float zoomMax, float zoomSpeed, float lerpSpeed, float moveSpeed) {
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        Vector2 delta = Vector2Scale(GetMouseDelta(), 1.0f / camera.zoom);
        camera.target = Vector2Subtract(camera.target, delta);
        centeredZoom = false;
    }

    float zoom = camera.zoom;
    float dt = GetFrameTime();
    camera.target.y -= dt*moveSpeed/zoom * IsKeyDown(KEY_UP);
    camera.target.y += dt*moveSpeed/zoom * IsKeyDown(KEY_DOWN);
    camera.target.x += dt*moveSpeed/zoom * IsKeyDown(KEY_RIGHT);
    camera.target.x -= dt*moveSpeed/zoom * IsKeyDown(KEY_LEFT);

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        if (!centeredZoom) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            // We first set the target to the mouse for proper zooming.
            camera.target = mouseWorldPos;
            // Now set the mouse position (=distance from top left) as offset
            // to restore the original view. Scrolling is now center around the mouse.
            camera.offset = GetMousePosition();
        }
        zoomTarget = Clamp(camera.zoom * (1+wheel*zoomSpeed), zoomMin, zoomMax);
    }

    camera.zoom = Lerp(camera.zoom, zoomTarget, lerpSpeed*dt);
}