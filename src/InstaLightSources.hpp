#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// created SL-200418
struct InstaPointLight {
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

// created SL-200418
struct InstaSpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec4 diffuse;
	float spotCutOff;
};

void LightSourcesInit();