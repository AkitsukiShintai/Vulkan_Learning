#pragma once
#include "Enums.h"
#include<vector>
#include<map>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm.hpp>
#include"BasicStruct.h"
#include<vulkan/vulkan.h>
#include "InternalStruct.h"
class Mesh {
public:
	void SetVertices(std::vector<Vertex_Aki> vertices,uint32_t submesh = 0);
	void SetIndices(std::vector<uint32_t> indicies, uint32_t submesh = 0);
	void UploadMesh();
	//TODO Get functions
	void AddSubmesh(Mesh* submesh);
	void SetTexture(const char* texturePath);
	Mesh();
	~Mesh();
private:
	friend class VulkanApplication;
	friend class ModelReader;
	std::vector<Vertex_Aki> mVertices;
	const char* m_Path;
	std::vector<uint32_t> mIndicies;	//submesh id, the indicies of the submesh
	std::vector<Mesh*> mSubmeshes;

	Image m_Texture;
	
	Buffer mVertexBuffer;
	Buffer mIndexBuffer;

};
