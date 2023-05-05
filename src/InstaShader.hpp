#pragma once
#include "OpenGL4.hpp"

#define POINT_LIGHTS_NUM 4

// created SL-200719
struct InstaShaderAttribsLocations {
	GLint aPosition = -1;
	GLint aColor = -1;
	GLint aNormal = -1;
	GLint aTangent = -1;
	GLint aTexCoord = -1;
	GLint aWeights = -1;
};

// created SL-200716
struct InstaShaderUniformLocations {
	// general opengl uniform locations
	GLint uModelMat = -1;
	GLint uViewMat = -1;
	GLint uProjMat = -1;
	GLint uShadowViewMat = -1;
	GLint uShadowProjMat = -1;
	GLint uBaseColor = -1;
	GLint uMask = -1;

	// point lights opengl uniform locations
	struct PointLight {
		GLint position = -1;
		GLint ambient = -1;
		GLint diffuse = -1;
		GLint specular = -1;
	} uPointLights[POINT_LIGHTS_NUM];

	// spot light opengl uniform locations
	struct SpotLight {
		GLint position = -1;
		GLint direction = -1;
		GLint diffuse = -1;
		GLint spotCutOff = -1;
	} uSpotLight;
};

// created SL-200716
struct InstaShaderTextureLocations {
	// general textures opengl uniforms locations
	GLint sBaseTexture = -1;
	GLint sDetailTexture = -1;
	GLint sNormalTexture = -1;
	GLint sShadowTexture = -1;

	// g-buffer texture opengl uniforms locations
	GLint sGBufferPosition = -1;
	GLint sGBufferTexCoord = -1;
	GLint sGBufferNormal = -1;
	GLint sGBufferColor = -1;
	GLint sGBufferLightSpacePos = -1;
	GLint sGBufferMask = -1;
	GLint sGBufferDepth = -1;
};

// created SL-200418
class InstaShader
{
public:
	// OpenGL handles and settings
	GLuint mGLVertexShader = 0;
	GLuint mGLFragmentShader = 0;
	GLuint mGLProgram = 0;

	// OpenGL shader params locations
	InstaShaderAttribsLocations mAttribsLocs;
	InstaShaderUniformLocations mUniformLocs;
	InstaShaderTextureLocations mTextureLocs;

public:
	// constructor and destructor
	InstaShader();
	~InstaShader();

	// create shader
	bool CreateShader(const char* vertexSource, const char* fragmentSource);
	void DeleteShader();

	// status check functions
	static bool ShaderStatusCheck(GLuint shader);
	static bool ProgramStatusCheck(GLuint program);
};