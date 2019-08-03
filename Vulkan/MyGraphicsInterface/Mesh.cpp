#include "VulkanApplication.h"
#include "Mesh.h"
#include "RenderPipeline.h"
#include<vulkan/vulkan.h>

void Mesh::SetVertices(std::vector<Vertex_Aki> vertices , uint32_t submesh) {
	mSubmeshes[submesh]->mVertices = vertices;
}

void Mesh::SetIndices(std::vector<uint32_t> indicies, uint32_t submesh) {
	mSubmeshes[submesh]->mIndicies = indicies;
}

void Mesh::UploadMesh() {
}

void Mesh::AddSubmesh(Mesh* submesh) {
	mSubmeshes.push_back(submesh);
}

void Mesh::SetTexture(const char* texturePath) {
	m_Path = texturePath;
}


Mesh::Mesh():mVertices(),mIndicies(),mSubmeshes() {
	mSubmeshes.push_back(this);
}
Mesh::~Mesh() {
	for (int i =1 ; i< mSubmeshes.size(); i++)
	{
		delete mSubmeshes[i];
	}
}