#pragma once
#include<vulkan/vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include<string>
#include<optional>
class Device {
	VkPhysicalDevice mPhysicalDevice;
	VkDevice mDevice;
	VkPhysicalDeviceFeatures mFeatures;
	VkPhysicalDeviceMemoryProperties mMemoryProperties;
	VkPhysicalDeviceProperties mProperties;
	VkPhysicalDeviceProperties mEnableProperties;
	std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;
	std::vector<std::string> mSupportedExtensions;
	VkCommandPool mCommandPool = VK_NULL_HANDLE;
	struct Familys {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> computeFamily;
		std::optional<uint32_t> transferFamily;
		std::optional<uint32_t> sparseBindingFamily;
		bool isComplete(unsigned int mQueueFlags);
	}mQueueFamilyIndices;


	struct Queues {
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkQueue computeQueue;
		VkQueue transferQueue;
		VkQueue sparseBindingQueue;
	}mQueues;


	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	operator VkDevice() { return mDevice; }

	Device(VkPhysicalDevice physicalDevice) {
		assert(physicalDevice);
		mPhysicalDevice = physicalDevice;
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &mProperties);
		vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mFeatures);
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
		vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
		assert(queueFamilyCount>0);
		mQueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, mQueueFamilyProperties.data());


	}
	~Device() {
		if (mCommandPool)
		{
			vkDestroyCommandPool(mDevice, mCommandPool,nullptr);
		}
		if (mDevice)
		{
			vkDestroyDevice(mDevice, nullptr);
		}
	
	}
};



class Image {

	VkImage mImage;
	VkImageLayout mImageLayout;
	VkBuffer mBuffer;
	
};

class Instance {
	VkInstance mInstance;
	

};









class GraphicsEngine {
public:
	static VkInstance CreateInstances();



};