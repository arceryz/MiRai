#include "Ray3Program.h"

Ray3Program::Ray3Program()
{
    shader = LoadShader("Shaders/ray3.vert", "Shaders/ray3.frag");
}
void Ray3Program::SetScene(Ray3Scene *_scene)
{
    scene = _scene;
}
void Ray3Program::Draw()
{
    model.materials[0].shader = shader;
    DrawModel(model, {}, 1.0f, WHITE);
}