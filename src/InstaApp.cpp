#include <iostream>
#include <array>
#include "InstaApp.hpp"
#include "InstaShaderSources.hpp"
#include "InstaStaticMeshes.hpp"
#include "InstaLoaders.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Created SL-200419
void InstaApp::Init() {
	// create shaders
	mMeshShader.CreateShader(cVSShaderSource_PositionNormalTexCoords, cFSShaderSource_PositionNormalTexCoords);
	mColorShader.CreateShader(cVSShaderSource_Position, cFSShaderSource_Position);
	mShadowShader.CreateShader(cVSShaderSource_Shadow, cFSShaderSource_Shadow);
	mGBufferShader.CreateShader(cVSShaderSource_GBuffer, cFSShaderSource_GBuffer);
	mBlitShader.CreateShader(cVSShaderSource_Blit, cFSShaderSource_Blit);
	// set point lights
	mPointLights[0].position = glm::vec4(+10.0f, +2.0f, +10.0f, 1.0f);
	mPointLights[1].position = glm::vec4(+10.0f, +2.0f, -10.0f, 1.0f);
	mPointLights[2].position = glm::vec4(-10.0f, +2.0f, +10.0f, 1.0f);
	mPointLights[3].position = glm::vec4(-10.0f, +2.0f, -10.0f, 1.0f);
	mPointLights[0].diffuse = glm::vec4(0.1f, 0.0f, 0.0f, 1.0f);
	mPointLights[1].diffuse = glm::vec4(0.0f, 0.1f, 0.0f, 1.0f);
	mPointLights[2].diffuse = glm::vec4(0.0f, 0.0f, 0.1f, 1.0f);
	mPointLights[3].diffuse = glm::vec4(0.1f, 0.1f, 0.0f, 1.0f);
	// set spot light
	mSpotLight.position = glm::vec3(+10.0f, +2.0f, +10.0f);
	mSpotLight.direction = glm::normalize(-mSpotLight.position);
	mSpotLight.diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	mSpotLight.spotCutOff = cosf(glm::radians(15.0f));
	
	// create depth map texture
	GL_CHECK(glGenTextures(1, &mDepthMapTex));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mDepthMapTex));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_TEX_SIZE, SHADOW_MAP_TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create depth map FBO
	GL_CHECK(glGenFramebuffers(1, &mDepthMapFBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMapTex, 0));
	GL_CHECK(glDrawBuffer(GL_NONE));
	GL_CHECK(glReadBuffer(GL_NONE));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	// create blit frame buffer
	// create G-Buffer FBO
	GL_CHECK(glGenFramebuffers(1, &mBlitFBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mBlitFBO));
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
	GL_CHECK(glDrawBuffers(sizeof(attachments) / sizeof(attachments[0]), attachments));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	// load some scene
	LoadObjFile("data/models/rock/rock.obj", "data/models/rock", mMeshes, mTextures);
	// create simple
	mPointLightMesh.CreateBuffers(cInstaQuadBuffer_Positions, cInstaQuadBuffer_Normals, cInstaQuadBuffer_TexCoords, 4);
	mPointLightMesh.mGLPrimitiveMode = GL_TRIANGLE_STRIP;
	mGBufferMesh.CreateBuffers(cInstaQuadBuffer_Positions, cInstaQuadBuffer_Normals, cInstaQuadBuffer_TexCoords, 4);
	mGBufferMesh.mGLPrimitiveMode = GL_TRIANGLE_STRIP;
	// load textures
	//LoadTextureFromFile("data/grass.jpg", &mTexture0);
	//LoadTextureFromFile("data/list.jpg", &mTexture1);
}

// Created SL-200419
void InstaApp::Destroy() {
	DestroyGBufferHandles();
	// destroy blit handles
	GL_CHECK(glDeleteFramebuffers(1, &mBlitFBO));
	// destroy depth map handles
	GL_CHECK(glDeleteTextures(1, &mDepthMapTex));
	GL_CHECK(glDeleteFramebuffers(1, &mDepthMapFBO));
	// destroy textures
	mTexture1.DestroyTexture();
	mTexture0.DestroyTexture();
	// destroy meshes
	mGBufferMesh.DestroyBuffers();
	mPointLightMesh.DestroyBuffers();
	// destroy textures
	for (auto& texture : mTextures)
		texture->DestroyTexture();
	// destroy meshes
	for (auto& mesh : mMeshes)
		mesh->DestroyBuffers();

	// destroy shaders
	mGBufferShader.DeleteShader();
	mShadowShader.DeleteShader();
	mColorShader.DeleteShader();
	mMeshShader.DeleteShader();
}

// Created SL-200419
void InstaApp::CreateGBufferHandles() {
	mMaskBuff.resize((size_t)((size_t)mViewportWidth * (size_t)mViewportHeight));
	mTexCoordBuff.resize((size_t)((size_t)mViewportWidth * (size_t)mViewportHeight));
	mNormalBuff.resize((size_t)((size_t)mViewportWidth * (size_t)mViewportHeight));
	// create G-Buffer FBO
	GL_CHECK(glGenFramebuffers(1, &mGBufferFBO));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mGBufferFBO));
	// create G-Buffer texture Position
	GL_CHECK(glGenTextures(1, &mGBufferTexPosition));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexPosition));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mViewportWidth, mViewportHeight, 0, GL_RGBA, GL_FLOAT, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGBufferTexPosition, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create G-Buffer texture tex coords
	GL_CHECK(glGenTextures(1, &mGBufferTexTexCoord));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexTexCoord));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, mViewportWidth, mViewportHeight, 0, GL_RG, GL_FLOAT, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGBufferTexTexCoord, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create G-Buffer texture Normal
	GL_CHECK(glGenTextures(1, &mGBufferTexNormal));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexNormal));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mViewportWidth, mViewportHeight, 0, GL_RGB, GL_FLOAT, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGBufferTexNormal, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create G-Buffer texture Color
	GL_CHECK(glGenTextures(1, &mGBufferTexColor));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexColor));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mViewportWidth, mViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mGBufferTexColor, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create G-Buffer texture Light Space Position
	GL_CHECK(glGenTextures(1, &mGBufferTexLightSpacePos));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexLightSpacePos));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mViewportWidth, mViewportHeight, 0, GL_RGBA, GL_FLOAT, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mGBufferTexLightSpacePos, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create G-Buffer texture Mask
	GL_CHECK(glGenTextures(1, &mGBufferTexMask));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexMask));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mViewportWidth, mViewportHeight, 0, GL_RED, GL_FLOAT, NULL));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, mGBufferTexMask, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// create G-Buffer depth map 
	GL_CHECK(glGenTextures(1, &mGBufferDepth));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferDepth));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mViewportWidth, mViewportHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGBufferDepth, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	// enable G-Buffer render targets
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
	GL_CHECK(glDrawBuffers(sizeof(attachments)/ sizeof(attachments[0]), attachments));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// Created SL-200419
void InstaApp::DestroyGBufferHandles() {
	// destroy G-Buffer handles
	GL_CHECK(glDeleteTextures(1, &mGBufferDepth));
	GL_CHECK(glDeleteTextures(1, &mGBufferTexMask));
	GL_CHECK(glDeleteTextures(1, &mGBufferTexColor));
	GL_CHECK(glDeleteTextures(1, &mGBufferTexNormal));
	GL_CHECK(glDeleteTextures(1, &mGBufferTexTexCoord));
	GL_CHECK(glDeleteTextures(1, &mGBufferTexPosition));
	GL_CHECK(glDeleteFramebuffers(1, &mGBufferFBO));
}

// Created SL-200719
void InstaApp::RenderPassShadowMap() {
	// shadow pass
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO));
	GL_CHECK(glViewport(0, 0, SHADOW_MAP_TEX_SIZE, SHADOW_MAP_TEX_SIZE));
	GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	// use shadow shader
	GL_CHECK(glUseProgram(mShadowShader.mGLProgram));
	GL_CHECK(glUniformMatrix4fv(mShadowShader.mUniformLocs.uViewMat, 1, GL_FALSE, glm::value_ptr(mShadowViewMatrix)));
	GL_CHECK(glUniformMatrix4fv(mShadowShader.mUniformLocs.uProjMat, 1, GL_FALSE, glm::value_ptr(mShadowProjMatrix)));
	// render meshes
	for (auto i = 0; i < mMeshes.size(); i++) {
		auto mesh = mMeshes[i];
		GL_CHECK(glBindVertexArray(mesh->mGLVertexArrayHandle));
		GL_CHECK(glUniformMatrix4fv(mShadowShader.mUniformLocs.uModelMat, 1, GL_FALSE, glm::value_ptr(mesh->mModelMatrix)));
		GL_CHECK(glDrawArrays(mesh->mGLPrimitiveMode, 0, mesh->mElementsCount));
		GL_CHECK(glBindVertexArray(0));
	}
	// unuse shadow shader
	GL_CHECK(glUseProgram(0));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// Created SL-200719
void InstaApp::RenderPassFillGBuffer() {
	// clear frame buffer
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mGBufferFBO));
	GL_CHECK(glViewport(0, 0, mViewportWidth, mViewportHeight));
	GL_CHECK(glClearColor(1.0f / 255.0f, 36.0f / 255, 86.0f / 255.0f, 1.0f));
	GL_CHECK(glClearDepth(1.0));
	GL_CHECK(glDisable(GL_CULL_FACE));
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// use mesh shader
	GL_CHECK(glUseProgram(mMeshShader.mGLProgram));
	GL_CHECK(glUniformMatrix4fv(mMeshShader.mUniformLocs.uViewMat, 1, GL_FALSE, glm::value_ptr(mViewMatrix)));
	GL_CHECK(glUniformMatrix4fv(mMeshShader.mUniformLocs.uProjMat, 1, GL_FALSE, glm::value_ptr(mProjMatrix)));
	GL_CHECK(glUniformMatrix4fv(mMeshShader.mUniformLocs.uShadowViewMat, 1, GL_FALSE, glm::value_ptr(mShadowViewMatrix)));
	GL_CHECK(glUniformMatrix4fv(mMeshShader.mUniformLocs.uShadowProjMat, 1, GL_FALSE, glm::value_ptr(mShadowProjMatrix)));
	// render meshes
	for (auto i = 0; i < mMeshes.size(); i++) {
		auto mesh = mMeshes[i];
		// unbind vertex array
		GL_CHECK(glBindVertexArray(mesh->mGLVertexArrayHandle));

		// set uniforms
		GL_CHECK(glUniform4fv(mMeshShader.mUniformLocs.uMask, 1, glm::value_ptr(glm::vec4(i + 1, 0.0f, 0.0f, 1.0f))));
		GL_CHECK(glUniformMatrix4fv(mMeshShader.mUniformLocs.uModelMat, 1, GL_FALSE, glm::value_ptr(mesh->mModelMatrix)));

		// bind texture and sampler
		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mesh->mDiffuseTextureHandle->mGLTextureHandle));
		GL_CHECK(glBindSampler(0, mesh->mDiffuseTextureHandle->mGLSamplerHandle));
		GL_CHECK(glUniform1i(mMeshShader.mTextureLocs.sBaseTexture, 0));

		// DRAW
		GL_CHECK(glDrawArrays(mesh->mGLPrimitiveMode, 0, mesh->mElementsCount));

		// unbind texture and sampler
		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
		GL_CHECK(glBindSampler(0, 0));
		GL_CHECK(glUniform1i(mMeshShader.mTextureLocs.sBaseTexture, 0));

		// unbind vertex array
		GL_CHECK(glBindVertexArray(0));
	}
	// unuse mesh shader
	GL_CHECK(glUseProgram(0));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// Created SL-200719
void InstaApp::RenderPassFinalGBuffer() {
	// render G-Buffer
	GL_CHECK(glViewport(0, 0, mViewportWidth, mViewportHeight));
	GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GL_CHECK(glClearDepth(1.0));
	GL_CHECK(glDisable(GL_CULL_FACE));
	GL_CHECK(glDisable(GL_DEPTH_TEST));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// enable shader
	GL_CHECK(glUseProgram(mGBufferShader.mGLProgram));

	// update point lights shader info
	for (auto i = 0; i < mPointLights.size(); i++) {
		GL_CHECK(glUniform4fv(mGBufferShader.mUniformLocs.uPointLights[i].position, 1, glm::value_ptr(mPointLights[i].position)));
		GL_CHECK(glUniform4fv(mGBufferShader.mUniformLocs.uPointLights[i].ambient, 1, glm::value_ptr(mPointLights[i].ambient)));
		GL_CHECK(glUniform4fv(mGBufferShader.mUniformLocs.uPointLights[i].diffuse, 1, glm::value_ptr(mPointLights[i].diffuse)));
		GL_CHECK(glUniform4fv(mGBufferShader.mUniformLocs.uPointLights[i].specular, 1, glm::value_ptr(mPointLights[i].specular)));
	}
	// update point lights shader info
	GL_CHECK(glUniform3fv(mGBufferShader.mUniformLocs.uSpotLight.position, 1, glm::value_ptr(mSpotLight.position)));
	GL_CHECK(glUniform3fv(mGBufferShader.mUniformLocs.uSpotLight.direction, 1, glm::value_ptr(mSpotLight.direction)));
	GL_CHECK(glUniform4fv(mGBufferShader.mUniformLocs.uSpotLight.diffuse, 1, glm::value_ptr(mSpotLight.diffuse)));
	GL_CHECK(glUniform1fv(mGBufferShader.mUniformLocs.uSpotLight.spotCutOff, 1, &mSpotLight.spotCutOff));

	// bind G-Buffer textures
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexPosition));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferPosition, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE1));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexTexCoord));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferTexCoord, 1));
	GL_CHECK(glActiveTexture(GL_TEXTURE2));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexNormal));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferNormal, 2));
	GL_CHECK(glActiveTexture(GL_TEXTURE3));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexColor));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferColor, 3));
	GL_CHECK(glActiveTexture(GL_TEXTURE4));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexLightSpacePos));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferLightSpacePos, 4));
	GL_CHECK(glActiveTexture(GL_TEXTURE5));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexMask));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferMask, 5));
	GL_CHECK(glActiveTexture(GL_TEXTURE6));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mDepthMapTex));
	GL_CHECK(glUniform1i(mGBufferShader.mTextureLocs.sGBufferDepth, 6));

	// draw mesh
	GL_CHECK(glBindVertexArray(mGBufferMesh.mGLVertexArrayHandle));
	GL_CHECK(glDrawArrays(mGBufferMesh.mGLPrimitiveMode, 0, mGBufferMesh.mElementsCount));
	GL_CHECK(glBindVertexArray(0));

	// bind G-Buffer textures
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE1));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE2));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE3));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE4));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE5));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE6));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glActiveTexture(GL_TEXTURE0));

	// disable shader
	GL_CHECK(glUseProgram(0));
}

// Created SL-200719
void InstaApp::RenderPassBlitTextures(InstaTexture& texDst, InstaTexture& texSrs, glm::vec2 center, glm::vec2 size) {
	// get texture properties
	GLint texHeight = 0, texWidth = 0;
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, texDst.mGLTextureHandle));
	GL_CHECK(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight));
	GL_CHECK(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

	// bind FBO and texture target
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mBlitFBO));
	GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texDst.mGLTextureHandle, 0));
	GL_CHECK(glViewport(0, 0, texWidth, texHeight));

	// create simple matrices
	glm::mat4 projMat = glm::ortho(0.0f, (float)texHeight, 0.0f, (float)texWidth);
	glm::mat4 modelMat =
		glm::translate(glm::vec3(texWidth * center.x, texHeight * center.y, 0.0f)) *
		glm::scale(glm::vec3(texWidth * size.x * 0.5f, texHeight * size.y * 0.5f, 0.0f));
	
	// use mesh shader
	GL_CHECK(glUseProgram(mBlitShader.mGLProgram));
	GL_CHECK(glUniformMatrix4fv(mBlitShader.mUniformLocs.uProjMat, 1, GL_FALSE, glm::value_ptr(projMat)));
	GL_CHECK(glUniformMatrix4fv(mBlitShader.mUniformLocs.uModelMat, 1, GL_FALSE, glm::value_ptr(modelMat)));

	// unbind vertex array
	GL_CHECK(glBindVertexArray(mGBufferMesh.mGLVertexArrayHandle));

	// bind texture and sampler
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, texSrs.mGLTextureHandle));
	GL_CHECK(glBindSampler(0, texSrs.mGLSamplerHandle));
	GL_CHECK(glUniform1i(mBlitShader.mTextureLocs.sBaseTexture, 0));

	// DRAW
	GL_CHECK(glDrawArrays(mGBufferMesh.mGLPrimitiveMode, 0, mGBufferMesh.mElementsCount));

	// unbind texture and sampler
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glBindSampler(0, 0));
	GL_CHECK(glUniform1i(mBlitShader.mTextureLocs.sBaseTexture, 0));

	// unbind vertex array
	GL_CHECK(glBindVertexArray(0));
	// unuse mesh shader
	GL_CHECK(glUseProgram(0));

	// unbind FBO and texture target
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// Created SL-200719
void InstaApp::RenderShowTexture(InstaTexture& texture) {
	// create simple matrices
	glm::mat4 projMat = glm::ortho(0.0f, 100.0f, 0.0f, 100.0f);
	glm::mat4 modelMat = 
		glm::translate(glm::vec3(50.0f, 50.0f, 0.0f)) *
		glm::scale(glm::vec3(50.0f, 50.0f, 50.0f));

	// clear frame buffer
	GL_CHECK(glViewport(0, 0, mViewportWidth, mViewportHeight));
	GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GL_CHECK(glClearDepth(1.0));
	GL_CHECK(glDisable(GL_CULL_FACE));
	GL_CHECK(glDisable(GL_DEPTH_TEST));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	// use mesh shader
	GL_CHECK(glUseProgram(mBlitShader.mGLProgram));
	GL_CHECK(glUniformMatrix4fv(mBlitShader.mUniformLocs.uProjMat, 1, GL_FALSE, glm::value_ptr(projMat)));
	GL_CHECK(glUniformMatrix4fv(mBlitShader.mUniformLocs.uModelMat, 1, GL_FALSE, glm::value_ptr(modelMat)));

	// unbind vertex array
	GL_CHECK(glBindVertexArray(mGBufferMesh.mGLVertexArrayHandle));

	// bind texture and sampler
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture.mGLTextureHandle));
	GL_CHECK(glBindSampler(0, texture.mGLSamplerHandle));
	GL_CHECK(glUniform1i(mBlitShader.mTextureLocs.sBaseTexture, 0));

	// DRAW
	GL_CHECK(glDrawArrays(mGBufferMesh.mGLPrimitiveMode, 0, mGBufferMesh.mElementsCount));

	// unbind texture and sampler
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	GL_CHECK(glBindSampler(0, 0));
	GL_CHECK(glUniform1i(mBlitShader.mTextureLocs.sBaseTexture, 0));

	// unbind vertex array
	GL_CHECK(glBindVertexArray(0));
	// unuse mesh shader
	GL_CHECK(glUseProgram(0));
}

// Created SL-200719
void InstaApp::GetSnapData() {
	// get mask value
	GL_CHECK(glActiveTexture(GL_TEXTURE0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexTexCoord));
	GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, mTexCoordBuff.data()));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexNormal));
	GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, mNormalBuff.data()));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, mGBufferTexMask));
	GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, mMaskBuff.data()));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

	// get snap data
	uint32_t index = (mViewportHeight - mCursorPosY - 1) * mViewportWidth + mCursorPosX;
	mSnapData.mTexCoord = mTexCoordBuff[index];
	mSnapData.mNormal = mNormalBuff[index];
	mSnapData.mMask = mMaskBuff[index];
	mSnapData.mMesh = nullptr;
	if (mSnapData.mMask > 0) 
		mSnapData.mMesh = mMeshes[(size_t)((size_t)mSnapData.mMask - 1)].get();
}

// Created SL-200419
void InstaApp::Render() {
// 	// render shadow map
 	RenderPassShadowMap();
// 	// fill g-buffer textures
 	RenderPassFillGBuffer();
// 	// blend all g-buffer textures - final render pass
 	RenderPassFinalGBuffer();
	//RenderPassBlitTextures();
	//RenderPassBlitTextures(mTexture0, mTexture1, glm::vec2(0.9f, 0.9f), glm::vec2(0.5f, 0.5f));
	//RenderPassBlitTextures(
// 		mTexture0, 
// 		mTexture1, 
// 		glm::vec2((float)mCursorPosX / mViewportWidth, 1.0f - (float)mCursorPosY / mViewportHeight),
// 		glm::vec2(0.05f, 0.05f));
	//RenderShowTexture(mTexture0);
	//RenderShowTexture(mTexture1);
	//RenderShowTexture(mTexture0);
	// get snap data
	//GetSnapData();
}

// UpdateMeshTexture
void InstaApp::UpdateMeshTexture() {
	if (mSnapData.mMesh && mSnapData.mMesh->mDiffuseTextureHandle) {
		// get texture parameters
		GLint texHeight = 0, texWidth = 0;
		GL_CHECK(glActiveTexture(GL_TEXTURE0));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mSnapData.mMesh->mDiffuseTextureHandle->mGLTextureHandle));
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		// read texture data
		std::vector<InstaColor> texData((size_t)texHeight * texWidth);
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mSnapData.mMesh->mDiffuseTextureHandle->mGLTextureHandle));
		GL_CHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data()));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

		// change tex data
		uint32_t centerX = (uint32_t)(mSnapData.mTexCoord.x * texWidth);
		uint32_t centerY = (uint32_t)(mSnapData.mTexCoord.y * texHeight);

		const int32_t QUAD_RANGE = (int32_t)mBrushRadius;
		for (int32_t x = -QUAD_RANGE; x < QUAD_RANGE; x++) {
			for (int32_t y = -QUAD_RANGE; y < QUAD_RANGE; y++) {
				int32_t pixelX = centerX + x;
				int32_t pixelY = centerY + y;
				int32_t index = pixelY * texWidth + pixelX;
				if ((index >= 0) && (index < texData.size())) {
					texData[index].r = mBrushColor.r;
					texData[index].g = mBrushColor.g;
					texData[index].b = mBrushColor.b;
				}
			}
		}

		// write tex data
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, mSnapData.mMesh->mDiffuseTextureHandle->mGLTextureHandle));
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData.data()));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}
}

// Created SL-200419
void InstaApp::Update(float deltaTime) {
	// update time
	static float newTime = 0;
	newTime += deltaTime;

	// update point lights positions
	for (auto i = 0; i < mPointLights.size(); i++)
		mPointLights[i].position = glm::rotate(glm::mat4(1.0f), newTime + i, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(10.0f, 3.0f, 10.0f, 1.0f);
	// set new camera mCameraDir
	mCameraDir = glm::normalize(glm::vec3(
		cos(glm::radians(mCameraPitch)) * cos(glm::radians(mCameraYaw)),
		sin(glm::radians(mCameraPitch)),
		cos(glm::radians(mCameraPitch)) * sin(glm::radians(mCameraYaw))
	));
	mSpotLight.position = glm::rotate(glm::mat4(1.0f), newTime/5.0f + 5, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(10.0f, 10.0f, 10.0f, 1.0f);
	mSpotLight.direction = -glm::normalize(mSpotLight.position);
	// update matrices
	mViewMatrix = glm::lookAt(mCameraEye, mCameraEye + mCameraDir, mCameraUp);
	mProjMatrix = glm::perspective(glm::radians(45.0f), (float)mViewportWidth / (float)mViewportHeight, 0.1f, 100.0f);
	// update shadow matrices
	mShadowViewMatrix = glm::lookAt(mSpotLight.position, mSpotLight.position + mSpotLight.direction, mCameraUp);
	mShadowProjMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
}

// Created SL-200419
void InstaApp::SetViewportSize(uint32_t viewportWidth, uint32_t viewportHeight) {
	if (viewportWidth == 0) viewportWidth = 1;
	if (viewportHeight == 0) viewportHeight = 1;
	if ((mViewportWidth != viewportWidth) | (mViewportHeight != viewportHeight)) {
		// store viewport size
		mViewportWidth = viewportWidth;
		mViewportHeight = viewportHeight;
		// recreate G-Buffer handles
		DestroyGBufferHandles();
		CreateGBufferHandles();
	}
}

// Created SL-200714
void InstaApp::SetCursorPosition(int32_t cursorPosX, int32_t cursorPosY)
{
	// update cursor position
	if (((mCursorPosX != cursorPosX) | (mCursorPosY != cursorPosY)) &&
		((cursorPosX < mViewportWidth) & (cursorPosY < mViewportHeight))) {
		mCursorPosX = cursorPosX;
		mCursorPosY = cursorPosY;
	}
}