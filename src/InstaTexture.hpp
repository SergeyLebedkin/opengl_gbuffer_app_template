#pragma once
#include <cinttypes>
#include "OpenGL4.hpp"

// created SL-201904
class InstaTexture {
public:
	// OpenGL texture handle
	GLuint mGLTextureHandle = 0;
	GLuint mGLSamplerHandle = 0;
public:
	// create buffers
	void CreateTexture(const uint8_t* data, uint32_t width, uint32_t height);
	void DestroyTexture();
};