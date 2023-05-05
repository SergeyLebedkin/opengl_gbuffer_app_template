#pragma once
#include <iostream>
#include <vector>
#include "InstaShader.hpp"

// InstaShader()
InstaShader::InstaShader()
{
}

// ~InstaShader()
InstaShader::~InstaShader()
{
	// simply delete shader
	DeleteShader();
}

// create CreateShader
bool InstaShader::CreateShader(const char* vertexSource, const char* fragmentSource)
{
	// create and compile vertex shader
	mGLVertexShader = GL_CHECK(glCreateShader(GL_VERTEX_SHADER));
	GL_CHECK(glShaderSource(mGLVertexShader, 1, &vertexSource, nullptr));
	GL_CHECK(glCompileShader(mGLVertexShader));
	if (!ShaderStatusCheck(mGLVertexShader))
	{
		GL_CHECK(glDeleteProgram(mGLVertexShader));
		mGLVertexShader = 0;
		return false;
	}

	// create and compile fragment shader
	mGLFragmentShader = GL_CHECK(glCreateShader(GL_FRAGMENT_SHADER));
	GL_CHECK(glShaderSource(mGLFragmentShader, 1, &fragmentSource, nullptr));
	GL_CHECK(glCompileShader(mGLFragmentShader));
	if (!ShaderStatusCheck(mGLFragmentShader))
	{
		GL_CHECK(glDeleteShader(mGLFragmentShader));
		GL_CHECK(glDeleteShader(mGLVertexShader));
		mGLFragmentShader = 0;
		mGLVertexShader = 0;
		return false;
	}

	// attach shaders
	mGLProgram = GL_CHECK(glCreateProgram());
	GL_CHECK(glAttachShader(mGLProgram, mGLVertexShader));
	GL_CHECK(glAttachShader(mGLProgram, mGLFragmentShader));

	// link program
	GL_CHECK(glLinkProgram(mGLProgram));
	if (!ProgramStatusCheck(mGLProgram))
	{
		GL_CHECK(glDeleteProgram(mGLProgram));
		GL_CHECK(glDeleteProgram(mGLFragmentShader));
		GL_CHECK(glDeleteShader(mGLVertexShader));
		mGLProgram = 0;
		mGLFragmentShader = 0;
		mGLVertexShader = 0;
		return false;
	}

	// use program
	GL_CHECK(glUseProgram(mGLProgram));

	// get attributes location
	mAttribsLocs.aPosition = GL_CHECK(glGetAttribLocation(mGLProgram, "aPosition"));
	mAttribsLocs.aColor = GL_CHECK(glGetAttribLocation(mGLProgram, "aColor"));
	mAttribsLocs.aNormal = GL_CHECK(glGetAttribLocation(mGLProgram, "aNormal"));
	mAttribsLocs.aTangent = GL_CHECK(glGetAttribLocation(mGLProgram, "aTangent"));
	mAttribsLocs.aTexCoord = GL_CHECK(glGetAttribLocation(mGLProgram, "aTexCoord"));
	mAttribsLocs.aWeights = GL_CHECK(glGetAttribLocation(mGLProgram, "aWeights"));

	// get uniforms location
	mUniformLocs.uModelMat = GL_CHECK(glGetUniformLocation(mGLProgram, "uModelMat"));
	mUniformLocs.uViewMat = GL_CHECK(glGetUniformLocation(mGLProgram, "uViewMat"));
	mUniformLocs.uProjMat = GL_CHECK(glGetUniformLocation(mGLProgram, "uProjMat"));
	mUniformLocs.uShadowViewMat = GL_CHECK(glGetUniformLocation(mGLProgram, "uShadowViewMat"));
	mUniformLocs.uShadowProjMat = GL_CHECK(glGetUniformLocation(mGLProgram, "uShadowProjMat"));
	mUniformLocs.uBaseColor = GL_CHECK(glGetUniformLocation(mGLProgram, "uBaseColor"));
	mUniformLocs.uMask = GL_CHECK(glGetUniformLocation(mGLProgram, "uMask"));

	// get OpenGL texture uniforms locations
	mTextureLocs.sBaseTexture = GL_CHECK(glGetUniformLocation(mGLProgram, "sBaseTexture"));
	mTextureLocs.sDetailTexture = GL_CHECK(glGetUniformLocation(mGLProgram, "sDetailTexture"));
	mTextureLocs.sNormalTexture = GL_CHECK(glGetUniformLocation(mGLProgram, "sNormalTexture"));
	mTextureLocs.sShadowTexture = GL_CHECK(glGetUniformLocation(mGLProgram, "sShadowTexture"));

	// get OpenGL G-Buffer texture uniforms locations
	mTextureLocs.sGBufferPosition = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferTexPosition"));
	mTextureLocs.sGBufferTexCoord = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferTexTexCoord"));
	mTextureLocs.sGBufferNormal = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferTexNormal"));
	mTextureLocs.sGBufferColor = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferTexColor"));
	mTextureLocs.sGBufferLightSpacePos = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferTexLightSpacePos"));
	mTextureLocs.sGBufferMask = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferMask"));
	mTextureLocs.sGBufferDepth = GL_CHECK(glGetUniformLocation(mGLProgram, "sGBufferDepth"));
	
	// point lights uniform locations
	for (auto i = 0; i < POINT_LIGHTS_NUM; i++) {
		GLchar uniformName[64];
		sprintf_s(uniformName, sizeof(uniformName), "uPointLights[%i].position", i);
		mUniformLocs.uPointLights[i].position = GL_CHECK(glGetUniformLocation(mGLProgram, uniformName));
		sprintf_s(uniformName, sizeof(uniformName), "uPointLights[%i].ambient", i);
		mUniformLocs.uPointLights[i].ambient = GL_CHECK(glGetUniformLocation(mGLProgram, uniformName));
		sprintf_s(uniformName, sizeof(uniformName), "uPointLights[%i].diffuse", i);
		mUniformLocs.uPointLights[i].diffuse = GL_CHECK(glGetUniformLocation(mGLProgram, uniformName));
		sprintf_s(uniformName, sizeof(uniformName), "uPointLights[%i].specular", i);
		mUniformLocs.uPointLights[i].specular = GL_CHECK(glGetUniformLocation(mGLProgram, uniformName));
	}
	// spot light uniform locations
	mUniformLocs.uSpotLight.position = GL_CHECK(glGetUniformLocation(mGLProgram, "uSpotLight.position"));
	mUniformLocs.uSpotLight.direction = GL_CHECK(glGetUniformLocation(mGLProgram, "uSpotLight.direction"));
	mUniformLocs.uSpotLight.diffuse = GL_CHECK(glGetUniformLocation(mGLProgram, "uSpotLight.diffuse"));
	mUniformLocs.uSpotLight.spotCutOff = GL_CHECK(glGetUniformLocation(mGLProgram, "uSpotLight.spotCutOff"));

	// use program
	GL_CHECK(glUseProgram(0));

	return true;
}

void InstaShader::DeleteShader()
{
	// delete shaders and program handle
	GL_CHECK(glDeleteProgram(mGLProgram));
	GL_CHECK(glDeleteShader(mGLFragmentShader));
	GL_CHECK(glDeleteShader(mGLVertexShader));
	mGLProgram = 0;
	mGLFragmentShader = 0;
	mGLVertexShader = 0;
}

// shader status check
bool InstaShader::ShaderStatusCheck(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		// get max length of info log
		GLsizei maxLength = 0;
		GLsizei length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &length, &infoLog[0]);
		std::cerr << infoLog.data() << std::endl;

		// Provide the info log in whatever manner you deem best.
		// Exit with failure.
		return false;
	}
	return true;
}

// ProgramStatusCheck
bool InstaShader::ProgramStatusCheck(GLuint program)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		// get max length of info log
		GLint maxLength = 0;
		GLsizei length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &length, &infoLog[0]);
		std::cerr << infoLog.data() << std::endl;

		// Provide the info log in whatever manner you deem best.
		// Exit with failure.
		return false;
	}
	return true;
}
