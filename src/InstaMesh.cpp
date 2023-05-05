#include "InstaMesh.hpp"

// CreateBuffers
void InstaMesh::CreateBuffers(const float* positions, const float* normals, const float* texCoords, GLsizeiptr elementsCount)
{
	assert(positions);
	assert(normals);
	assert(texCoords);

	// store elements count
	mElementsCount = (GLsizei)elementsCount;

	// create and fill position buffer
	GL_CHECK(glGenBuffers(1, &mGLPositionBuffer));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mGLPositionBuffer));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elementsCount * 3, positions, GL_STATIC_DRAW));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	// create and fill normal buffer
	GL_CHECK(glGenBuffers(1, &mGLNormalBuffer));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mGLNormalBuffer));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elementsCount * 3, normals, GL_STATIC_DRAW));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	// create and fill tex coords buffer
	GL_CHECK(glGenBuffers(1, &mGLTexCoordBuffer));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mGLTexCoordBuffer));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elementsCount * 2, texCoords, GL_STATIC_DRAW));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// create and setup VAO
	GL_CHECK(glGenVertexArrays(1, &mGLVertexArrayHandle));
	GL_CHECK(glBindVertexArray(mGLVertexArrayHandle));
	// enable add position buffer to VAO
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mGLPositionBuffer));
	GL_CHECK(glVertexAttribPointer(cSLVertexAttribLocation_Position, 3, GL_FLOAT, GL_FALSE, 0, 0));
	GL_CHECK(glEnableVertexAttribArray(cSLVertexAttribLocation_Position));
	// enable add position buffer to VAO
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mGLNormalBuffer));
	GL_CHECK(glVertexAttribPointer(cSLVertexAttribLocation_Normal, 3, GL_FLOAT, GL_FALSE, 0, 0));
	GL_CHECK(glEnableVertexAttribArray(cSLVertexAttribLocation_Normal));
	// enable add position buffer to VAO
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mGLTexCoordBuffer));
	GL_CHECK(glVertexAttribPointer(cSLVertexAttribLocation_TexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0));
	GL_CHECK(glEnableVertexAttribArray(cSLVertexAttribLocation_TexCoord));
	// unbind VAO
	GL_CHECK(glBindVertexArray(0));
}

// DestroyBuffers
void InstaMesh::DestroyBuffers() {
	GL_CHECK(glDeleteBuffers(1, &mGLTexCoordBuffer));
	GL_CHECK(glDeleteBuffers(1, &mGLNormalBuffer));
	GL_CHECK(glDeleteBuffers(1, &mGLPositionBuffer));
	GL_CHECK(glDeleteVertexArrays(1, &mGLVertexArrayHandle));
}
