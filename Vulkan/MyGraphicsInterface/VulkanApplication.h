#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define __VULKAN_APPLICATION
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Enums.h"
#include"BasicStruct.h"
#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <glfw3.h>
#include"Application.h"
#include "RenderPipeline.h"
#include "Mesh.h"
#include<array>
class VulkanApplication :public Application {

public:
	glm::vec3 mCamera;
	glm::vec3 mCameraForward;
public:
	VulkanApplication(std::string applicationName, int width, int height);
	VulkanApplication();
public:
	void Init() override;
	void Run() override;
	void SetResizable(bool state) override;

	//help function;
	void SetApplicationInfo(std::string applicationName, int width, int height) override;
	void SetRequiredValadationLayers(std::vector<const char*> valadationLayers)override;
	void SetRequiredExtentions(std::vector<const char*> extentions)override;
	void SetWindowHandle(HWND window, HINSTANCE handle)override;
	void SetDeviceExtensions(std::vector<const char*> extentions)override;
	void SetQueueFlag(QueueFlagBits flag);
	void ProcessInput(GLFWwindow* window);
private:
	struct Familys {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> computeFamily;
		std::optional<uint32_t> transferFamily;
		std::optional<uint32_t> sparseBindingFamily;
		bool isComplete(unsigned int mQueueFlags);
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

private:
	GLFWwindow* mWindow;

	std::vector<const char*> mValidationLayers;
	std::vector<const char*> mRequiredExtentions;
	std::vector<const char*> mDeviceExtensions;
	unsigned int mQueueFlags;
	Familys mFamily;
	bool mEnableValidationLayer;

	int MAX_FRAMES_IN_FLIGHT = 2;

	size_t currentFrame = 0;


	VkInstance mInstance;

	//debug related
	VkDebugUtilsMessengerEXT mDebugMessenger;
	PFN_vkDebugUtilsMessengerCallbackEXT mDebugCallback;

	//surface
	VkSurfaceKHR mSurface;
	HWND mHWND;
	HINSTANCE mHINSTANCE;

	//Device related
	VkPhysicalDevice mPhysicalDevice;
	VkDevice mLogicDevice;
	Queues mQueues;

	//swap chain related
	VkSwapchainKHR mSwapChain;
	std::vector<VkImage>mSwapChainImages;
	std::vector<VkImageView>mSwapChainImageViews;
	VkFormat mSwapChainImageFormat;
	VkExtent2D mSwapChainExtent;

	//Render pass related
	bool mFramebufferResized = false;
	VkRenderPass mRenderPass;
	VkFramebuffer mSwapChainFramebuffer;

	//first stage
	VkPipeline mPipeline1;
	VkPipelineLayout mPipelineLayout1;

	
	VkPipeline mPipeline2;
	VkPipelineLayout mPipelineLayout2;




	VkCommandPool mCommandPool;
	VkCommandBuffer mCommandBuffer;
	
	std::vector<VkSemaphore>mImageAvailableSemaphores;
	std::vector<VkSemaphore>mRenderFinishedSemaphores;
	std::vector<VkFence>mInFlightFences;

	VkDescriptorSetLayout mDescriptorSetLayout;

	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;

	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;


	VkBuffer mUniformBuffers;
	VkDeviceMemory mUniformBuffersMemory;

	VkDescriptorPool mDescriptorPool;
	VkDescriptorSet mDescriptorSets;


	VkImage mTextureImage;
	VkDeviceMemory mTextureImageMemory;
	VkImageView mTextureImageView;
	VkSampler mTextureSampler[2];

private:
	std::map<std::string, VkRenderPass> mRenderPasses;
	std::map<VkRenderPass, std::vector<VkPipeline>> mPipelines;
	std::map<VkRenderPass, std::vector<VkPipelineLayout>> mPipelineLayouts;
	std::vector<VkFramebuffer> mFramebuffers;
	//pass -- mesh
	std::map<VkRenderPass, std::vector<Mesh*>>  mRenderPass_Meshes;

public:
	//help functions
	VkInstance GetInstance();
	VkDevice GetDevice();
	VkRenderPass GetRenderPass();
	VkExtent2D GetSwipchainExtent();
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

private:
	void MainLoop();
	void Cleanup();
	void CleanupSwapChain();
	void CreateMyWindow();

	void CreateInstance();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();
	void CreateDebugCallback();
	void CreateSurface();


	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	Familys FindQueueFamilies(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	void CreateDevice();
	void GetQueues(Familys queueFamily);

	void CreateSwapChain();
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateImageViews();
	VkImageView CreateImageView(VkImage image, VkFormat format);

	void CreateRenderPass();
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void RecreateSwapChain();
	void UpdateUniformBuffer(uint32_t currentImage);
	void CreateTextureImage(const char* filePath);
	void CreateTextureImageView();
	void CreateTextureSampler();
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);


	template<class T>
	void CreateVertexBuffer(std::vector<T>vertexdata, VkRenderPass pipeline);

	void UploadMesh(std::string renderPassName, Mesh* mesh, int submesh = 0) override;

	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();

	void CreateDescriptorSetLayout();

	void DrawFrame();



};