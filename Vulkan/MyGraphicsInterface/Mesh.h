#pragma once
#include "Enums.h"
#include<vector>
#include<map>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm.hpp>
#include"BasicStruct.h"
#include<vulkan/vulkan.h>
class Mesh {
public:
	void SetVertices(std::vector<Vertex_Aki> vertices);
	void SetIndices(std::vector<uint32_t> indicies, uint32_t submesh);
	void UploadMesh();
	//TODO Get functions
	void AddSubmesh(Mesh* submesh);
	Mesh();
	~Mesh();
private:
	friend class VulkanApplication;
	friend class ModelReader;
	std::vector<Vertex_Aki> mVertices;

	std::vector<uint32_t> mIndicies;	//submesh id, the indicies of the submesh
	std::vector<Mesh*> mSubmeshes;

	IndexType mIndexType;
	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;

	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;

	void CreateVertexBuffer();
	void CreateIndexBuffer();


};