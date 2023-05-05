#pragma once

#include <glm/mat4x4.hpp>
#include "InstaTexture.hpp"

// created SL-201904
class InstaMesh {
private:
	// OpenGL buffers handle
	GLuint mGLPositionBuffer = 0;
	GLuint mGLColorBuffer = 0;
	GLuint mGLNormalBuffer = 0;
	GLuint mGLTexCoordBuffer = 0;
public:
	// textures
	InstaTexture* mDiffuseTextureHandle = nullptr;
	// OpenGL VAO handle
	GLuint mGLVertexArrayHandle = 0;
	// OpenGL mesh settings
	GLuint mGLPrimitiveMode = GL_TRIANGLES;
	GLsizei mElementsCount = 0;
	// model matrix
	glm::mat4 mModelMatrix = glm::mat4(1.0f);

	// create buffers
	void CreateBuffers(const float* positions, const float* normals, const float* texCoords, GLsizeiptr elementsCount);
	void DestroyBuffers();
};