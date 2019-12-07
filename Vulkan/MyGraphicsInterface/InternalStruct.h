#pragma once
#include <vulkan/vulkan.h>
#include<optional>
#include<vector>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include"glm.hpp"
struct Image {
	VkImage m_Image;
	VkImageView m_ImageView;
	VkDeviceMemory m_Memory;
	VkImageLayout m_CurrentLayout;
	VkSampler m_Sampler;
	VkDescriptorImageInfo m_ImageInfo;
	VkFormat m_Format;
};

struct Buffer {
	VkBuffer m_Buffer;
	VkDeviceSize m_Size;
	VkDeviceMemory m_Memory;
	void* m_Data;
	VkDescriptorBufferInfo m_BufferInfo;
};

struct Pipeline {
	VkPipelineLayout m_Layout;
	VkPipeline m_Pipeline;
	friend bool operator < (const struct Pipeline& ls, const struct Pipeline& rs);
};
inline bool operator < (const struct Pipeline& ls, const struct Pipeline& rs) {
	return false;	
}
struct Model {
	Buffer m_Vertex;
	Buffer m_Index;
	uint32_t m_IndexSize;
	Image m_Texture;
	Buffer m_Modeling;
	glm::mat4x4 m_ModlingMat;

};


struct Familys {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> transferFamily;
	std::optional<uint32_t> sparseBindingFamily;
	bool isComplete(unsigned int m_QueueFlags);
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


struct Queues {
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;
	VkQueue transferQueue;
	VkQueue sparseBindingQueue;
};
