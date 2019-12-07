#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define __VULKAN_APPLICATION
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "Enums.h"
#include"BasicStruct.h"
#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <glfw3.h>
#include"Application.h"
#include "RenderPipeline.h"
#include "Mesh.h"
#include<array>
#include<unordered_set>
#include <unordered_map>
#include "ObjectPool.h"
#include "InternalStruct.h"
#include <algorithm>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include "gtx/quaternion.hpp"
class VulkanApplication :public Application {

private:


public:
	VulkanApplication(std::string applicationName, int width, int height);
	VulkanApplication();
public:
	void Init();
	void Run();
	void SetResizable(bool state);

	//help function;
	void SetApplicationInfo(std::string applicationName, int width, int height);
	void SetRequiredValadationLayers(std::vector<const char*> valadationLayers)override;
	void SetRequiredExtentions(std::vector<const char*> extentions)override;
	void SetWindowHandle(HWND window, HINSTANCE handle)override;
	void SetDeviceExtensions(std::vector<const char*> extentions)override;
	void SetQueueFlag(QueueFlagBits flag);
	void ProcessInput(GLFWwindow* window) {};
private:

private:
	GLFWwindow* m_Window;

	std::vector<const char*> m_ValidationLayers;
	std::vector<const char*> m_RequiredExtentions;
	std::vector<const char*> m_DeviceExtensions;
	unsigned int m_QueueFlags;
	Familys m_Family;
	bool m_EnableValidationLayer;

	int MAX_FRAMES_IN_FLIGHT = 2;

	size_t currentFrame = 0;


	VkInstance m_Instance;

	//debug related
	VkDebugUtilsMessengerEXT m_DebugMessenger;
	PFN_vkDebugUtilsMessengerCallbackEXT m_DebugCallback;

	//surface
	VkSurfaceKHR m_Surface;
	HWND m_HWND;
	HINSTANCE m_HINSTANCE;

	//Device related
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_LogicDevice;
	Queues m_Queues;

	//swap chain related
	VkSwapchainKHR m_SwapChain;
	std::vector<Image> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;

	bool m_FramebufferResized = false;

	VkCommandPool m_CommandPool;
	LightInfo sunLight;
	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;


	VkDescriptorPool m_DescriptorPool;

	Image m_RenderTarget;
	Buffer m_CameraBuffer;

	Image m_DepthBuffer;

	Image m_WhiteImage;
	VkSampler m_TextureSampler;

	//#define TEXTURE_ARRAY_SIZE 8
		//opaque texture array
		//VkDescriptorImageInfo	m_OpaqueDescriptorImageInfos[TEXTURE_ARRAY_SIZE];

private:
	friend class Application;
	//std::vector<Pipeline> mPip
	//std::unordered_map<size_t, std::vector<VkDescriptorSet>> m_Pipeline_DescriptorSets;
	//std::unordered_map<size_t, std::vector<Pipeline>> m_RenderPass_Pipelines;
	//std::unordered_map<size_t,VkFramebuffer> m_RenderPass_Framebuffers;
	////pass -- m_esh
	//std::unordered_map<size_t, std::vector<Model>>  m_Pipeline_Models;
	////pass -- descriptor set layout
	//std::unordered_map<size_t, VkDescriptorSetLayout>  m_Pipeline_DescriptorSetLayout;
	VkSampler m_DifferSampler;
	////0 skybox
	////1 opaque
	////2 differ 1
	////3 differ 2
	////4 ssr
	std::array<VkRenderPass, 5> m_RenderPasses;
	Pipeline skyboxPipeline;
	VkDescriptorSet skyboxDescriptorSet;
	Model skyboxModel;
	VkFramebuffer skyboxFramebuffer;
	VkDescriptorSetLayout skyboxDescriptorSetLayout;

	Pipeline opaquePipeline;
	std::vector<VkDescriptorSet> opaqueDescriptorSet;
	VkFramebuffer opaqueFramebuffer;
	std::vector<Model> opaqueModels;
	VkDescriptorSetLayout opaqueDescriptorSetLayout;

	//differ render
	Pipeline differPipelines[2];
	Image normalImage;
	Image colorImage;
	Image positionImage;
	VkFramebuffer differ_framebuffer[2];
	Buffer light;
	VkDescriptorSetLayout differ_descriptorSetLayouts[2];
	Model full_screen_rect;
	std::vector<Model> differModels;
	std::vector<VkDescriptorSet> differDescriptorSets;
	VkDescriptorSet differ_secondStageDescriptorSet;

	//SSR
	Pipeline SSRPipelines;
	VkFramebuffer SSR_framebuffer;
	VkDescriptorSetLayout SSR_descriptorSetLayout;
	std::vector<Model> SSRModels;
	std::vector<VkDescriptorSet> SSRDescriptorSets;
	VkDescriptorSet SSR_DescriptorSet;
	Image SSR_RenderTarget;

	//shadow
	VkRenderPass shadowPass;
	Pipeline shadowPipeline;
	VkDescriptorSetLayout shadowLayout;
	std::vector<VkDescriptorSet> shadowSets;
	std::vector<Model> shadowModels;
	Buffer lightCameraBuffer;
	VkFramebuffer shadow_Frambuffer;
	Image shadowDepth;
	/*ObjectPool<VkRenderPass, 50> m_RenderPassPool;
	ObjectPool<Image, 200> m_ImagePool;
	ObjectPool<Pipeline, 200> m_PipelinePool;
	ObjectPool<Buffer, 1000> m_BufferPool;
	ObjectPool<VkSampler, 20> m_SamplerPool;
	ObjectPool<VkFramebuffer, 200> m_FramebufferPool;
	ObjectPool<VkDescriptorSet, 1000> m_DescriptorSetPool;*/
public:
	//help functions

	VkPipelineDepthStencilStateCreateInfo CreateDepthStencilStateCreateInfo(
		VkBool32                                  depthTestEnable = VK_FALSE,
		VkBool32                                  depthWriteEnable = VK_FALSE,
		VkCompareOp                               depthCompareOp = VK_COMPARE_OP_LESS,
		float                                     minDepthBounds = 0,
		float                                     maxDepthBounds = 1,
		VkBool32                                  depthBoundsTestEnable = VK_FALSE,
		VkBool32                                  stencilTestEnable = VK_FALSE,
		VkStencilOpState                          front = {},
		VkStencilOpState                          back = {},
		VkPipelineDepthStencilStateCreateFlags    flags = 0);

	VkStencilOpState CreateStencilOpState(
		VkStencilOp    failOp,
		VkStencilOp    passOp,
		VkStencilOp    depthFailOp,
		VkCompareOp    compareOp,
		uint32_t       compareMask,
		uint32_t       writeMask,
		uint32_t       reference);

	Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void MapBuffer(Buffer& buffer);
	void UnMapBuffer(Buffer& buffer);
	void CopyDataToBuffer(void* data, Buffer& buffer, uint32_t size = 0);
	void CopyBufferToBuffer(VkCommandBuffer cmd, Buffer srcBuffer, Buffer dstBuffer);
	void SetDescriptorImageInfo(Image& image, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	Image CreateCubeMap(std::vector<std::string> files);
	Image CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageType type = VK_IMAGE_TYPE_2D, uint32_t layers = 1, VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
	void CreateImageView(Image& image, VkFormat format, VkImageAspectFlags flag = VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D);
	VkFramebuffer CreateFrameBuffer(std::vector<Image> images, VkRenderPass renderPass, uint32_t width, uint32_t height);
	VkPipelineShaderStageCreateInfo CreateShader(std::string name, VkShaderStageFlagBits stage, const char* enterFunction = "main");
	Pipeline CreatePipeline(VkRenderPass renderPass,
		std::vector<VkPipelineShaderStageCreateInfo> stages,
		std::vector<VkVertexInputBindingDescription> bindings,
		std::vector<VkVertexInputAttributeDescription> attributes,
		std::vector<VkDescriptorSetLayout> laytous,
		VkPipelineDepthStencilStateCreateInfo depthInfo,
		uint32_t subpass,
		std::vector < VkPipelineColorBlendStateCreateInfo> colorBlendings = {},
		std::vector<VkViewport> viewports = {},
		std::vector <VkRect2D> scissors = {},
		std::vector < VkPipelineRasterizationStateCreateInfo> rasterizationStates = {},
		std::vector < VkPipelineMultisampleStateCreateInfo> m_ultisamplings = {},
		std::vector < VkDynamicState> dynamicStates = {}
	);
	void CopyBufferToImage(VkCommandBuffer cmd, uint32_t width, uint32_t height, Buffer srcBuffer, Image& dstImage, std::vector<VkBufferImageCopy> region = {});
	void CopyImageToImage(VkCommandBuffer cmd, uint32_t width, uint32_t height, Image& srcImage, Image& dstImage);
	VkAttachmentDescription CreateAttachmentDescriptionForColorAttachment(VkFormat format,
		VkImageLayout startLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout endLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE);
	VkAttachmentDescription CreateAttachmentDescriptionForDepthAttachment(VkFormat format,
		VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout startLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		VkImageLayout endLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);


	void SetSubpassDescriptionColorAttachment(VkSubpassDescription& description, VkAttachmentReference* attachmentsRef, uint32_t size);
	void SetSubpassDescriptionDepthAttachment(VkSubpassDescription& description, const VkAttachmentReference& attachmentRef);
	void SetSubpassDescriptionDepthAttachment(VkSubpassDescription& description, VkAttachmentReference* attachmentsRef, uint32_t size);
	void SetSubpassDescriptionBindPoint(VkSubpassDescription& description, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);





	VkSubpassDependency CreateSubpassDependency(uint32_t srcSubpass,
		uint32_t                dstSubpass,
		VkPipelineStageFlags    srcStageMask,
		VkPipelineStageFlags    dstStageMask,
		VkAccessFlags           srcAccessMask,
		VkAccessFlags           dstAccessMask,
		VkDependencyFlags       dependencyFlags = 0);
	VkRenderPass CreateRenderPass(std::vector< VkAttachmentDescription> attachments, std::vector< VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> dependencies);

	void TransitionImageLayout(VkCommandBuffer cmd, Image& image, VkImageLayout newLayout, VkImageSubresourceRange subSourceRange = { VK_IMAGE_ASPECT_COLOR_BIT ,0,1,0,1 });

	VkWriteDescriptorSet CreateWriteDescriptorSetForBuffer(const Buffer& buffer, uint32_t buinding, VkDescriptorSet descriptorSet);
	VkWriteDescriptorSet CreateWriteDescriptorSetForImage(const Image& image, uint32_t buinding, VkDescriptorSet descriptorSet);

	void Present(Image& image, uint32_t swapChainID, VkSemaphore* signalSemaphores);
	VkCommandBuffer CreateCommandBuffer(bool start = true);
	void DestroyBuffer(Buffer& buffer);

	void UpdateUniformBuffer();
	Image CreateTextureImage(const char* filePath);

	VkSampler CreateTextureSampler();
	void SetSampler(Image& image, VkSampler sampler);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void Prepare();

	Buffer CreateVertexBuffer(std::vector<Vertex_Aki>vertexdata);
	Buffer CreateIndexBuffer(std::vector<uint32_t>indices);

	VkVertexInputBindingDescription CreateVertexInputBindingDescription(uint32_t binding, VkVertexInputRate inputRate, uint32_t stride = sizeof(Vertex_Aki));
	VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(uint32_t  location, uint32_t binding, VkFormat format, uint32_t offset = 0);

	VkDescriptorSetAllocateInfo CreateDescriptorSetAllocateInfo(VkDescriptorSetLayout* descriptorSetLayouts, uint32_t count = 1);
	void UpdateDescriptorSets(std::vector<VkWriteDescriptorSet> writeDescriptors);

	VkDescriptorSet CreateDescriptorSet(std::vector<VkDescriptorSetAllocateInfo> allocateInfo);


	void CreateDescriptorPool();
	VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags, uint32_t descriptorCount = 1);

	VkDescriptorSetLayout CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> binds);
	VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState(VkBool32 blendEnable = VK_FALSE,
		VkBlendFactor            srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		VkBlendFactor            dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		VkBlendOp                colorBlendOp = VK_BLEND_OP_ADD,
		VkBlendFactor            srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		VkBlendFactor            dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		VkBlendOp                alphaBlendOp = VK_BLEND_OP_ADD,
		VkColorComponentFlags    colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	VkPipelineColorBlendStateCreateInfo CreatePipelineColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState* colorBlendAttachments, size_t cout, VkBool32 logicOpEnable = VK_FALSE, VkLogicOp logicOp = VK_LOGIC_OP_COPY);


	void DrawFrame(const VkCommandBuffer& cmd, Image& presentImage);
	void SwapImageToScreen() {};

	void BeginRenderPass(const VkCommandBuffer& cmd, const VkRenderPass renderPass, const VkFramebuffer& framebuffer, const std::vector<VkClearValue>& clearColors, VkExtent2D area);
	void BeginPipeline(const VkCommandBuffer& cmd, const Pipeline& pipeline, VkViewport viewPort = {}, VkRect2D scissor = {});
	void DrawMesh(const VkCommandBuffer& cmd, const Model& model, const Pipeline& pipeline, const VkDescriptorSet* descriptorSet, size_t descriptorSetCount = 1);


	GLFWwindow* GetWindow() override;
public:
	void MainLoop() {};
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
	void CreateSwapChainImageViews();
	void CreateCommandPool();

	void CreateSyncObjects();
	void RecreateSwapChain();
	void CreateGlobleVeriable();
	VkSampler CreateClampSamper();

	Model UploadMesh(Mesh* m_esh, int submesh = 0);
	void UploadOpaqueMesh(Mesh* mesh, int submesh);

	void SkyboxPrepare();
	void OpaquePrepare();
	void OpaqueUpdate();

	void DifferPassPrepare();
	void DifferPassUpdate();
	void UploadDifferMesh(Mesh* mesh, int submesh);

	void SSRPassPrepare();
	void SSRPassUpdate();
	void UploadSSRMesh(Mesh* mesh, int submesh);


	void ShadowPrepare();
	void ShadowUpdate();

	//get camera corners in camera space
	//fov: degree
	std::array<glm::vec3,4> CaluculateCameraCorners(const Camera& camera, float step);

	template<class T>
	T SmoothStep(const T& edge0, const T& edge1, const T& value) {
		T t;
		t = std::clamp((value - edge0) / (edge1 - edge0), 0.0, 1.0);
		return t * t * (3.0 - 2.0 * t);
	}




	glm::quat FromTo(const glm::vec3& from, const glm::vec3& to) {
		glm::quat q;
		glm::vec3 a = glm::cross(glm::normalize(from), glm::normalize(to));
		q.x = a.x;
		q.y = a.y;
		q.z = a.z;
		q.w = 1.0f + glm::dot(from, to);
		return q;
		//glm::mat4x4 mat = (glm::mat4x4)q;
	}

	template<class T>
	T Lerp(const T& a, const T& b, float t) {		
		return a + t * (b - a);
	}


	//According to camera corners to generate light camera frustum
	//need 8 corners
	///viewMat : camera view matrix
	glm::mat4x4 CalcuLightCameraFrustum(const CameraCorner& cameraCornersInCameraCoord, const glm::mat4x4& viewMat, LightInfo light);
	const float LIGHT_CASCADES_STEP[5] = {0.0f, 0.067f,0.067f + 0.133f , 0.067f + 0.133f + 0.267f,1.0f };

	void CalculateFourLightCameras(const CameraCorner& cameraCornersInCameraCoord, const CameraViewInfo& camera, LightInfo light, glm::mat4* results);

	std::array<glm::mat4x4, 4> CalculateCameraCascadesFrustms(const Camera& camera, const LightInfo& light);

	void WaterPrepare();
	void LoadWaterMesh(Mesh* mesh, uint32_t submesh);
};