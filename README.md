# Application
The application implements a deferred renderer that is capable of rendering 3D primitives, 4 colored point-light sources and a white single spotlight.
The point-light sources are not casting shadows. The spotlight should cast a filtered shadow.
The camera movable by moving the mouse with pushed left button and translatable in the scene using the arrow keys or a typical WASD-configuration plus EQ-configuration as Up-Down moving. 
Scene lighting renders by integrating the Phong shading model.
The application use free 3D models in obj format with textures.
The application can be compiled with or Microsoft Visual Studio 2019 or later.
The 3D framework is OpenGL 4.1. For context setup a helper library GLFW used.

# Dependencies
Used technologies and frameworks:
* Visual Studio 2019
* OpenGL 4.1
* glfw (https://github.com/glfw/glfw) - GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development
* glm (https://github.com/g-truc/glm) - OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
* stb (https://github.com/nothings/stb) - single-file public domain (or MIT licensed) libraries for C/C++. Used for texture loading
* tinyobjloader (https://github.com/syoyo/tinyobjloader) - Tiny but powerful single file wavefront obj loader written in C++
All dependencies and assets already included into repository, you don’t need to download them.

# Compile
The program can be compiled using Visual Studio 2019 (2017 altho supported).
Solution "InstaLOD.sln" file can be founded in .buils/vs2019 folder.
All neccesary assets can be founded in "data" folder.
Resulted executables can be founded in ".bin" folder.
To run standalone executable, file must be placed to the same directory as data folder.