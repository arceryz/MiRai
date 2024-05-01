# Mirai 2D/3D Flat/Curved Mirror Simulator

<img align="left" style="width:260px" src="https://github.com/arceryz/Mirai/blob/master/demo3d.gif" width="288px">
<img align="left" style="width:260px" src="https://github.com/arceryz/Mirai/blob/master/demo2d.gif" width="288px">

**A 2D + 3D mirror box visualiser on the GPU**.

The purpose of this project is to analyse the patterns from mirror reflections in scenes with lines and planes in 2D and 3D, with an extension to convex and concave curved mirrors. This tool is part of my Bachelor's Thesis.

*This program requires raylib to be compiled with `GRAPHIC_API=GRAPHICS_API_OPENGL_43`, otherwise you won't be able to use compute shaders.*

---

This mirror raytracer works with scenes that are described by 2D lines or 3D polygons. Rays will be cast from the view point and in 2D the resulting image points are drawn.

In 3D you get the first-person perspective of the rays, which is not possible in 2D (it would become a 1D image). The engine converts 3D triangle meshes to polygons for mirrors. All faces that are connected and have the same normal become a single mirror in the shader.

- The /Ray2 directory holds the source for the 2D raytracer and 2D scene format.
- THe /Ray3 diirectory holds the source for the 3D raytracer and 3D scene format.
- Both directories contain a single header Ray2.h or Ray3.h that contains the GUI.
- The main.cpp file switches between the two programs.

The shaders are likewise split in 2D and 3D variants.
- The 2D raytracer uses a compute shader to store the image point distances in a buffer.
- The 3D raytracer uses no buffer and computes color in the fragment shader.
- The fundamental difference is that 2D raytracer is not visualising the 1D camera's perspective, but the 3D raytracer is. This makes the 3D raytracer a bit simpler.