#pragma once

#include <array>
#include <vector>
#include <memory>
#include "InstaMesh.hpp"
#include "InstaTexture.hpp"
#include "InstaShader.hpp"
#include "InstaLightSources.hpp"

#define SHADOW_MAP_TEX_SIZE 2048

// created SL-200714
struct InstaColor {
	uint8_t r, g, b, a;
};

// created SL-200714
struct InstaSnapData {
	// mesh
	InstaMesh* mMesh;
	// mesh tex coords
	glm::vec2 mTexCoord;
	// mesh normal
	glm::vec3 mNormal;
	// mesh mask
	float mMask;
};

// created SL-200419
class InstaApp
{
private:
	// shaders
	InstaShader mMeshShader;
	InstaShader mColorShader;
	InstaShader mShadowShader;
	InstaShader mGBufferShader;
	InstaShader mBlitShader;
	// meshes
	InstaMesh mPointLightMesh;
	InstaMesh mGBufferMesh;
	std::vector<std::shared_ptr<InstaMesh>> mMeshes;
	// textures
	InstaTexture mTexture0;
	InstaTexture mTexture1;
	std::vector<std::shared_ptr<InstaTexture>> mTextures;
private:
	// blit FBO
	GLuint mBlitFBO = 0;
	// shadow map
	GLuint mDepthMapFBO = 0;
	GLuint mDepthMapTex = 0;
	// G-Buffer
	GLuint mGBufferFBO = 0;
	GLuint mGBufferTexPosition = 0;
	GLuint mGBufferTexTexCoord = 0;
	GLuint mGBufferTexNormal = 0;
	GLuint mGBufferTexColor = 0;
	GLuint mGBufferTexLightSpacePos = 0;
	GLuint mGBufferTexMask = 0;
	GLuint mGBufferDepth = 0;
private:
	std::vector<glm::vec2> mTexCoordBuff;
	std::vector<glm::vec3> mNormalBuff;
	std::vector<float> mMaskBuff;
private:
	InstaSnapData mSnapData;
private:
	// window parameters
	uint32_t mViewportWidth = 0;
	uint32_t mViewportHeight = 0;
	// cursor position
	int32_t mCursorPosX = 0;
	int32_t mCursorPosY = 0;
private:
	// brush settings
	float mBrushRadius = 0;
	InstaColor mBrushColor;
private:
	void CreateGBufferHandles();
	void DestroyGBufferHandles();
private:
	void UpdateMeshTexture();
public:
	// point lights
	std::array<InstaPointLight, POINT_LIGHTS_NUM> mPointLights{};
	// spot light
	InstaSpotLight mSpotLight;
	// camera settings
	glm::vec3 mCameraEye = glm::vec3(0.0f, 0.0f, 15.0f);
	glm::vec3 mCameraDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	// camera angles
	float mCameraPitch = 0.0f;
	float mCameraYaw = -90.0f;
	float mCameraSpeed = 10.0f;

	// view and projection matrices
	glm::mat4 mViewMatrix = glm::mat4(1.0f);
	glm::mat4 mProjMatrix = glm::mat4(1.0f);
	// shadow view and projection matrices
	glm::mat4 mShadowViewMatrix = glm::mat4(1.0f);
	glm::mat4 mShadowProjMatrix = glm::mat4(1.0f);
private:
	// render shadow map pass
	void RenderPassShadowMap();
	// fill g-buffer textures render pass
	void RenderPassFillGBuffer();
	// blend all g-buffer textures render pass
	void RenderPassFinalGBuffer();
	// copy texture one to another
	void RenderPassBlitTextures(InstaTexture& texDst, InstaTexture& texSrs, glm::vec2 center, glm::vec2 size);
	// copy texture one to another
	void RenderShowTexture(InstaTexture& texture);
	// get snap data
	void GetSnapData();
public:
	InstaApp() {};
	virtual ~InstaApp() {};

	// main functions
	void Init();
	void Destroy();
	void Render();
	void Update(float deltaTime);

	// settings
	void SetViewportSize(uint32_t viewportWidth, uint32_t viewportHeight);
	void SetCursorPosition(int32_t cursorPosX, int32_t cursorPosY);

	// brush
	void ApplyBrush() { UpdateMeshTexture(); };
	void SetBrushRadius(float radius) { mBrushRadius = radius; };
	void SetBrushColor(glm::vec4 color) { 
		mBrushColor.r = (uint8_t)(color.r * 255);
		mBrushColor.g = (uint8_t)(color.g * 255);
		mBrushColor.b = (uint8_t)(color.b * 255);
		mBrushColor.a = (uint8_t)(color.a * 255);
	};
};