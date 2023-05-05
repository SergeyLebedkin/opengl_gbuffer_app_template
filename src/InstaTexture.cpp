#include "InstaTexture.hpp"

// created SL-200418
void InstaTexture::CreateTexture(const uint8_t* data, uint32_t width, uint32_t height) {
	// create sample handle 
	GL_CHECK(glGenSamplers(1, &mGLSamplerHandle));
	GL_CHECK(glSamplerParameteri(mGLSamplerHandle, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CHECK(glSamplerParameteri(mGLSamplerHandle, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL_CHECK(glSamplerParameteri(mGLSamplerHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glSamplerParameteri(mGLSamplerHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
	// create texture handle
	GL_CHECK(glGenTextures(1, &mGLTextureHandle));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGLTextureHandle));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

// created SL-200418
void InstaTexture::DestroyTexture() {
	GL_CHECK(glDeleteSamplers(1, &mGLSamplerHandle));
	GL_CHECK(glDeleteTextures(1, &mGLTextureHandle));
}
