#include "InstaLoaders.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>
#include <glm/gtx/transform.hpp>

// created SL-201804
void LoadObjFile(
	std::string fileName, std::string basePath,
	std::vector<std::shared_ptr<InstaMesh>>& meshes,
	std::vector<std::shared_ptr<InstaTexture>>& textures)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn = "";
	std::string err = "";
	tinyobj::LoadObj(&attribs, &shapes, &materials, &warn, &err, fileName.c_str(), basePath.c_str(), true);
	std::cout << warn << std::endl;
	std::cout << err << std::endl;

	// load materials
	std::map<std::string, std::shared_ptr<InstaTexture>> texMap;
	for (const auto& material : materials) {
		auto texture = std::make_shared<InstaTexture>();
		std::string imageDiffuseFilePath = basePath + material.diffuse_texname;
		LoadTextureFromFile(imageDiffuseFilePath, texture.get());
		// store new texture
		texMap[material.diffuse_texname] = texture;
		textures.push_back(texture);
	}

	// allocate buffers
	std::vector<float> vecVertexBuffer = {};
	std::vector<float> vecNormaldBuffer = {};
	std::vector<float> vecTexCoordBuffer = {};
	for (size_t i = 0; i < shapes.size(); i++)
	{
		tinyobj::shape_t& shape = shapes[i];
		// create buffers
		for (tinyobj::index_t& index : shape.mesh.indices)
		{
			// if vertex exists
			if (index.vertex_index >= 0) {
				vecVertexBuffer.push_back(attribs.vertices[(size_t)3 * index.vertex_index + 0]);
				vecVertexBuffer.push_back(attribs.vertices[(size_t)3 * index.vertex_index + 1]);
				vecVertexBuffer.push_back(attribs.vertices[(size_t)3 * index.vertex_index + 2]);
			}

			// if normal exists
			if (index.normal_index >= 0) {
				vecNormaldBuffer.push_back(attribs.normals[(size_t)3 * index.normal_index + 0]);
				vecNormaldBuffer.push_back(attribs.normals[(size_t)3 * index.normal_index + 1]);
				vecNormaldBuffer.push_back(attribs.normals[(size_t)3 * index.normal_index + 2]);
			}

			// if texCoords exists
			if (index.texcoord_index >= 0) {
				vecTexCoordBuffer.push_back(attribs.texcoords[(size_t)2 * index.texcoord_index + 0]);
				vecTexCoordBuffer.push_back(attribs.texcoords[(size_t)2 * index.texcoord_index + 1]);
			}
		}

		std::cout << shape.name << std::endl;
		std::cout << materials[shape.mesh.material_ids[0]].diffuse_texname << std::endl;

		// create mesh
		auto mesh = std::make_shared<InstaMesh>();
		mesh->CreateBuffers(vecVertexBuffer.data(), vecNormaldBuffer.data(), vecTexCoordBuffer.data(), vecVertexBuffer.size() / 3);
		mesh->mDiffuseTextureHandle = texMap[materials[shape.mesh.material_ids[0]].diffuse_texname].get();
		mesh->mModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / 1000.0f));
		mesh->mGLPrimitiveMode = GL_TRIANGLES;
		meshes.push_back(mesh);

		// clear vector buffers
		vecVertexBuffer = {};
		vecNormaldBuffer = {};
		vecTexCoordBuffer = {};
	}
}

// created SL-201804
void LoadTextureFromFile(std::string filePath, InstaTexture* texure) {
	assert(texure);
	// load image data from file
	int width = 0, height = 0, channels = 0;
	stbi_uc* texData = stbi_load(filePath.data(), &width, &height, &channels, 4);
	texure->CreateTexture(texData, width, height);
	// free image data
	stbi_image_free(texData);
}