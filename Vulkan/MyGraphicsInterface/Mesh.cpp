#include "VulkanApplication.h"
#include "Mesh.h"
#include "RenderPipeline.h"
#include<vulkan/vulkan.h>

void Mesh::SetVertices(std::vector<Vertex_Aki> vertices) {
	mVertices = vertices;
}

void Mesh::SetIndices(std::vector<uint32_t> indicies, uint32_t submesh) {
	mIndicies = indicies;
	
	mIndexType = INDEX_TYPE_UINT32;

}

void Mesh::UploadMesh() {
	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Mesh::AddSubmesh(Mesh* submesh) {
	mSubmeshes.push_back(submesh);
}

Mesh::Mesh():mVertices(),mIndexType(INDEX_TYPE_UINT32),mIndicies(),mSubmeshes() {
	mSubmeshes.push_back(this);
}
Mesh::~Mesh() {
	for (int i =1 ; i< mSubmeshes.size(); i++)
	{
		delete mSubmeshes[i];
	}
}

void Mesh::CreateVertexBuffer() {
	VulkanApplication* app = static_cast<VulkanApplication*>(Application::GetInstance());
	VkDeviceSize bufferSize = sizeof(Vertex_Aki) * mVertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	app->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(app->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(app->GetDevice(), stagingBufferMemory);

	app->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

	app->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

	vkDestroyBuffer(app->GetDevice(), stagingBuffer, nullptr);
	vkFreeMemory(app->GetDevice(), stagingBufferMemory, nullptr);
	for (int i =1 ; i< mSubmeshes.size();i++)
	{
		mSubmeshes[i]->CreateVertexBuffer();
	}

}

void Mesh::CreateIndexBuffer() {
	VulkanApplication* app = static_cast<VulkanApplication*>(Application::GetInstance());

	VkDeviceSize bufferSize = sizeof(mIndicies[0]) * mIndicies.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	app->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(app->GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mIndicies.data(), (size_t)bufferSize);
	vkUnmapMemory(app->GetDevice(), stagingBufferMemory);

	app->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

	app->CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

	vkDestroyBuffer(app->GetDevice(), stagingBuffer, nullptr);
	vkFreeMemory(app->GetDevice(), stagingBufferMemory, nullptr);
	for (int i = 1; i < mSubmeshes.size(); i++)
	{
		mSubmeshes[i]->CreateIndexBuffer();
	}
}
