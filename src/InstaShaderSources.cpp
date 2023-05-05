#include "InstaShaderSources.hpp"
#include <string>

// vertex shader with position
const char* cVSShaderSource_Position = R"(
#version 430 core

// attributes
layout (location = 0) in vec3 aPosition;

// uniforms matrices
uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

// outputs
layout (location = 1) out vec4 vColor;

// main
void main()
{
	// find position
	gl_Position = uProjMat * uViewMat * uModelMat * vec4(aPosition, 1.0);
}

)";

// vertex shader with position
const char* cFSShaderSource_Position = R"(
#version 430 core

// inputs
uniform vec4 uBaseColor;

// outputs
layout (location = 0) out vec4 fragColor;

// main
void main()
{
	fragColor = uBaseColor;
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

)";

// ============================================================================

// vertex shader depth map
const char* cVSShaderSource_Shadow = R"(
#version 430 core

// attributes
layout (location = 0) in vec3 aPosition;

// uniforms matrices
uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

// main
void main()
{
	gl_Position = uProjMat * uViewMat * uModelMat * vec4(aPosition, 1.0);
}

)";

// fragment shader depth map
const char* cFSShaderSource_Shadow = R"(

#version 430 core

// outputs
layout (location = 0) out vec4 fragColor;

void main()
{
	// Do NOT NEED TO DRAW ENYTHING, JUST DEPTH!
	// fragColor = gl_FragCoord.z;
}

)";

// ============================================================================

// vertex shader with position, normal and texCoords
const char* cVSShaderSource_PositionNormalTexCoords = R"(
#version 430 core

// attributes
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec3 aNormal;
layout (location = 4) in vec2 aTexCoord;

// uniforms matrices
uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;
uniform mat4 uShadowViewMat;
uniform mat4 uShadowProjMat;

// outputs
layout (location = 0) out vec4 vWorldPos;
layout (location = 2) out vec3 vNormal;
layout (location = 4) out vec2 vTexCoord;
layout (location = 6) out vec4 vLightSpacePos;

// main
void main()
{
	// get normal matrix
	mat4 normalMatrix = transpose(inverse(uModelMat));

	// copy in to out
	vWorldPos = uModelMat * vec4(aPosition, 1.0);
	vNormal = normalize(normalMatrix * vec4(aNormal, 0.0)).xyz;
	vTexCoord = aTexCoord;

	// find position
	vLightSpacePos = uShadowProjMat * uShadowViewMat * uModelMat * vec4(aPosition, 1.0);
	gl_Position = uProjMat * uViewMat * uModelMat * vec4(aPosition, 1.0);
}

)";

// fragment shader with position, normal, color and texCoords
const char* cFSShaderSource_PositionNormalTexCoords = R"(

#version 430 core

// inputs
layout (location = 0) in vec4 vWorldPos;
layout (location = 2) in vec3 vNormal;
layout (location = 4) in vec2 vTexCoord;
layout (location = 6) in vec4 vLightSpacePos;

// textures
layout (binding = 0) uniform sampler2D sBaseTexture;

// uniforms mask
uniform vec4 uMask;

// outputs
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec2 gTexCoords;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec4 gColor;
layout (location = 4) out vec4 gLightSpacePosition;
layout (location = 5) out vec4 gMask;

// main
void main()
{
	gPosition = vWorldPos;
	gTexCoords = vTexCoord;
	gNormal = vNormal;
	gColor = texture2D(sBaseTexture, vTexCoord.xy);
	gLightSpacePosition = vLightSpacePos;
	gMask = uMask;
}

)";

// ============================================================================

// vertex shader depth map
const char* cVSShaderSource_GBuffer = R"(
#version 430 core

// attributes
layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec2 aTexCoord;

// outputs
layout (location = 4) out vec2 vTexCoord;

// main
void main()
{
	vTexCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

)";

// fragment shader depth map
const char* cFSShaderSource_GBuffer = R"(

#version 430 core

// inputs
layout (location = 4) in vec2 vTexCoord;

// textures
layout (binding = 0) uniform sampler2D sGBufferTexPosition;
layout (binding = 1) uniform sampler2D sGBufferTexTexCoord;
layout (binding = 2) uniform sampler2D sGBufferTexNormal;
layout (binding = 3) uniform sampler2D sGBufferTexColor;
layout (binding = 4) uniform sampler2D sGBufferTexLightSpacePos;
layout (binding = 5) uniform sampler2D sGBufferMask;
layout (binding = 6) uniform sampler2D sGBufferDepth;

// outputs
layout (location = 0) out vec4 fragColor;

// uniform point lights
#define POINT_LIGHTS_NUM 4
struct PointLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform PointLight uPointLights[POINT_LIGHTS_NUM];

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec4 diffuse;
	float spotCutOff;
};
uniform SpotLight uSpotLight;

void main()
{
	// get data form g-buffer
	vec4 vWorldPos = texture2D(sGBufferDepth, vTexCoord.xy);
	vec2 vMeshTexCoord = texture2D(sGBufferTexTexCoord, vTexCoord.xy).xy;
	vec3 vNormal = texture2D(sGBufferTexNormal, vTexCoord.xy).xyz;
	vec4 vLightSpacePos = texture2D(sGBufferTexLightSpacePos, vTexCoord.xy);
	vec4 vDiffuseTex = texture2D(sGBufferTexColor, vTexCoord.xy);
	vec4 vMask = texture2D(sGBufferMask, vTexCoord.xy);

	// diffuse point lights
	vec4 diffusePointLight = vec4(0.0);
	for (int i = 0; i < POINT_LIGHTS_NUM; i++) {
		vec3 dirPointLight = normalize(uPointLights[i].position.xyz - vWorldPos.xyz).xyz;
		diffusePointLight += uPointLights[i].diffuse * max(dot(vNormal, dirPointLight), 0.0);
	}

	// diffuse spot lights
	vec3 dirSpotLight = normalize(vWorldPos.xyz - uSpotLight.position).xyz;
	float cutoffSpotLight = dot(dirSpotLight, uSpotLight.direction);
	float intenseSpotLight = cutoffSpotLight < uSpotLight.spotCutOff ? 0.0 : 1.0;
	vec4 diffuseSpotLight = uSpotLight.diffuse * intenseSpotLight;

	// shadowIntence
	vec3 shadowPos = vLightSpacePos.xyz / vLightSpacePos.w * 0.5 + 0.5;
	float shadowDepth = texture(sGBufferDepth, shadowPos.xy).r;
	float shadowIntence = shadowPos.z - 0.005 < shadowDepth ? 1.0 : 0.5;

	// final color
	fragColor = vec4((vDiffuseTex + diffusePointLight).xyz * shadowIntence, 1.0);
	//fragColor = vec4(vNormal, 1.0);
	//fragColor = vec4(vMeshTexCoord.xy, 1.0, 1.0);
	//fragColor = vec4(vMask.xyz/40.0, 1.0);
}

)";

// ============================================================================

// vertex shader depth map
const char* cVSShaderSource_Blit = R"(
#version 430 core

// attributes
layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec2 aTexCoord;

// uniforms matrices
uniform mat4 uModelMat;
uniform mat4 uProjMat;

// outputs
layout (location = 4) out vec2 vTexCoord;

// main
void main()
{
	vTexCoord = aTexCoord;
	gl_Position = uProjMat * uModelMat * vec4(aPosition, 1.0);
}

)";

// fragment shader depth map
const char* cFSShaderSource_Blit = R"(

#version 430 core

// inputs
layout (location = 4) in vec2 vTexCoord;

// textures
layout (binding = 0) uniform sampler2D sBaseTexture;

// outputs
layout (location = 0) out vec4 fragColor;

void main()
{
	vec4 vDiffuseTex = texture2D(sBaseTexture, vTexCoord.xy);
	fragColor = vDiffuseTex;
}

)";