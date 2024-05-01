# Mirai Mirror Simulator

<img align="left" style="width:260px" src="https://github.com/arceryz/Mirai/blob/master/demo3d.gif" width="288px">
<img align="left" style="width:260px" src="https://github.com/arceryz/Mirai/blob/master/demo2d.gif" width="288px">

**A 2D + 3D mirror box visualiser on the GPU**.

The purpose of this project is to analyse the patterns from mirror reflections in scenes with lines and planes in 2D and 3D, with an extension to convex and concave curved mirrors. This tool is part of my Bachelor's Thesis.

*You can get raygui.h [here](https://github.com/raysan5/raygui/blob/master/src/raygui.h)*

---
This program requires raylib to be compiled with `GRAPHIC_API=GRAPHICS_API_OPENGL_43`, otherwise you won't be able to use compute shaders.