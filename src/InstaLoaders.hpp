#pragma once
#include "InstaMesh.hpp"
#include <vector>
#include <string>
#include <memory>

// LoadObjFile
void LoadObjFile(
	std::string fileName, std::string basePath,
	std::vector<std::shared_ptr<InstaMesh>>& meshes,
	std::vector<std::shared_ptr<InstaTexture>>& texures);

// LoadTextureFromFile
void LoadTextureFromFile(std::string filePath, InstaTexture* textures);