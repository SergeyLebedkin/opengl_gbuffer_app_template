#pragma once

// vertex shader with position
extern const char* cVSShaderSource_Position;
// fragment shader with position
extern const char* cFSShaderSource_Position;

// vertex shader depth map
extern const char* cVSShaderSource_Shadow;
// fragment shader depth map
extern const char* cFSShaderSource_Shadow;

// vertex shader with position, normal and tex coords
extern const char* cVSShaderSource_PositionNormalTexCoords;
// fragment shader with position, normal and tex coords
extern const char* cFSShaderSource_PositionNormalTexCoords;

// vertex shader with position
extern const char* cVSShaderSource_GBuffer;
// fragment shader with position
extern const char* cFSShaderSource_GBuffer;

// vertex shader for texture copy
extern const char* cVSShaderSource_Blit;
// fragment shader for texture copy
extern const char* cFSShaderSource_Blit;
