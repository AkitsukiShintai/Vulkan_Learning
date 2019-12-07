#include"VulkanApplication.h"
#include <assert.h>
#include<iostream>
#include <stdexcept>
#include<set>
#include<chrono>
#include <stdarg.h>
#include <fstream>
#include<array>
#include"Mesh.h"
#include<stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include"ModelReader.h"
#include <filesystem>
#define COMPILE_SHADER
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define R VK_FORMAT_R8G8B8A8_UNORM
//#define B VK_FORMAT_B8G8R8A8_UNORM
#define  COLOR_ATTACHMENT_FORMAT VK_FORMAT_R8G8B8A8_UNORM
#define  DEPTH_ATTACHMENT_FORMAT VK_FORMAT_D32_SFLOAT_S8_UINT
#define DATA_ATTACHMENT_FORMAT VK_FORMAT_R32G32B32A32_SFLOAT
#define MOUSE_CALLBACK
Camera camera;
//camera info
//{x = 26.2728043 y = 32.1282730 z = -23.1095695 ...}
glm::vec3 pos = { 0.0f,2.0f,10.0f };
//glm::vec3 pos = { 26.27280f,32.128f,-23.109f};
glm::vec3 dir = { 0.0f,0.0f,-1.0f };
//glm::vec3 dir = { -1.0f,-1.0f,-1.0f };
glm::vec3 up = { 0,1,0 };
bool keys[1024];
double m_ouseSpeed = 0.1f;
float deltaTime = 0.0f;   // 当前帧遇上一帧的时间差
float lastFrame = 0.0f;   // 上一帧的时间
float yaw, pitch;
float lastX = 400, lastY = 400;
bool firstMouse = true;
float fov = 60;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int m_ods)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 110.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 110.0f)
		fov = 110.0f;
}

static void m_ouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) // 这个bool变量一开始是设定为true的
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}


	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标的范围是从下往上的
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	dir = glm::normalize(front);
}
static void Do_movement() {

	float cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		pos += cameraSpeed * dir;
	if (keys[GLFW_KEY_S])
		pos -= cameraSpeed * dir;
	if (keys[GLFW_KEY_A])
		pos -= glm::normalize(glm::cross(dir, up)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		pos += glm::normalize(glm::cross(dir, up)) * cameraSpeed;

}

//const std::vector<Vertex_Aki> vertices = {
//	{{-0.5f, -0.5f,0}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//	{{0.5f, -0.5f,0}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//	{{0.5f, 0.5f,0}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//	{{-0.5f, 0.5f,0}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
//};
//
//const std::vector<uint32_t> indices = {
//	0, 1, 2, 2, 3, 0
//};


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT m_essageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT m_essageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (m_essageType & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		//assert(false);
	}
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::exception("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	file.clear();
	return buffer;

}




VulkanApplication::VulkanApplication(std::string applicationName, int width, int height)
	: m_ValidationLayers({
		"VK_LAYER_KHRONOS_validation"
		})
	, m_RequiredExtentions()
	, m_DeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME })
#ifdef NDEBUG
	, m_EnableValidationLayer(false)
#else
	, m_EnableValidationLayer(true)
#endif
	, m_DebugCallback(nullptr)
	, m_Window(nullptr)
{

};

VulkanApplication::VulkanApplication() :Application("", 0, 0)
, m_ValidationLayers({
	"VK_LAYER_KHRONOS_validation"
	})
	, m_RequiredExtentions()
	, m_DeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME })
#ifdef NDEBUG
	, m_EnableValidationLayer(false)
#else
	, m_EnableValidationLayer(true)
#endif
	, m_DebugCallback(nullptr)
	, m_Window(nullptr)
	//, m_RenderPass_Pipelines()
	, m_RenderPasses()
	//, m_RenderPass_Framebuffers()
	//, m_Pipeline_DescriptorSets()
	//, m_Pipeline_Models()
{

}

void VulkanApplication::Init() {
	if (m_QueueFlags == 0)
	{
		m_QueueFlags = QUEUE_COMPUTE_BIT | QUEUE_GRAPHICS_BIT | QUEUE_TRANSFER_BIT;
	}

	if (m_Window == nullptr)
	{
		CreateMyWindow();
	}

#ifdef MOUSE_CALLBACK
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(m_Window, key_callback);
	glfwSetCursorPosCallback(m_Window, m_ouse_callback);
#endif
	glfwSetScrollCallback(m_Window, scroll_callback);
	CreateInstance();
	CreateDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateDevice();
	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateSyncObjects();
	//CreateDescriptorSetLayout();
	CreateDescriptorPool();
	CreateCommandPool();
	CreateGlobleVeriable();

	Prepare();
}

VkSampler VulkanApplication::CreateClampSamper() {

	VkSampler sap;
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;


	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(m_LogicDevice, &samplerInfo, nullptr, &sap) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
	return sap;

}


void VulkanApplication::CreateGlobleVeriable() {
	camera.fov = 60.0f;
	camera.aspect = m_Width / (float)m_Height;
	camera.nearClip = 0.1f;
	camera.farClip = 1000.0f;
	camera.viewInfo = {};
	m_CameraBuffer = CreateBuffer(sizeof(CameraViewInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	MapBuffer(m_CameraBuffer);
	m_RenderTarget = CreateImage(m_Width, m_Height, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
	m_TextureSampler = CreateTextureSampler();
	m_DifferSampler = CreateClampSamper();
	SetSampler(m_RenderTarget, m_DifferSampler);
	CreateImageView(m_RenderTarget, VK_FORMAT_B8G8R8A8_UNORM);
	VkCommandBuffer cmd = CreateCommandBuffer();
	TransitionImageLayout(cmd, m_RenderTarget, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	for (size_t i = 0; i < m_SwapChainImages.size(); i++)
	{
		TransitionImageLayout(cmd, m_SwapChainImages[i], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}


	//create depth buffer
	m_DepthBuffer = CreateImage(m_Width, m_Height, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	CreateImageView(m_DepthBuffer, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT);
	TransitionImageLayout(cmd, m_DepthBuffer, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	//create white color image for tex
	m_WhiteImage = CreateTextureImage("..\\textures\\white.png");
	SetSampler(m_WhiteImage, m_TextureSampler);
	
	SetDescriptorImageInfo(m_DepthBuffer,VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
	SetSampler(m_DepthBuffer, m_DifferSampler);

	//load plain mesh
	Mesh* mesh = new Mesh();
	ModelReader::ReadModule("..\\models\\plane.obj", mesh);
	mesh->m_Path = "..\\textures\\texture.jpg";
	full_screen_rect = UploadMesh(mesh);

	//create light buffer
	light = CreateBuffer(sizeof(LightInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	MapBuffer(light);
	sunLight = {};
	sunLight.ambientStrenth = 0.1f;
	sunLight.lightColor = glm::vec4(1,1,1,0);
	sunLight.lightPos = glm::vec3(100, 100, 100);
	sunLight.lightDir = glm::vec3(-1.0, -1.0, -1.0);
	sunLight.type = DIRECT_LIGHT;
	CopyDataToBuffer(&sunLight, light);
	UnMapBuffer(light);

	//shadow light camera
	lightCameraBuffer = CreateBuffer(sizeof(glm::mat4x4)*4, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	MapBuffer(lightCameraBuffer);

	//shadow depth
	shadowDepth = CreateImage(4096, 4096, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	CreateImageView(shadowDepth, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_IMAGE_ASPECT_DEPTH_BIT);
	TransitionImageLayout(cmd, shadowDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	SetSampler(shadowDepth, m_DifferSampler);
	SetDescriptorImageInfo(shadowDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
	EndSingleTimeCommands(cmd);

}

void VulkanApplication::Prepare() {

	//skybox

	//SkyboxPrepare();

	//Opaque pass
	//OpaquePrepare();

	//differ
	DifferPassPrepare();

	ShadowPrepare();
	//SSR
	//SSRPassPrepare();
}


void VulkanApplication::Run() {
	Do_movement();
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	auto cmd = CreateCommandBuffer();
	//skybox
	VkClearValue textureClearColor = { 0,0,0,0 };
	VkClearValue depthClearColor = { 1,0x00 };
	/*BeginRenderPass(cmd, m_RenderPasses[0], skyboxFramebuffer, { textureClearColor });
	BeginPipeline(cmd, skyboxPipeline);
	DrawMesh(cmd, skyboxModel, skyboxPipeline, &skyboxDescriptorSet);
	vkCmdEndRenderPass(cmd);*/

	//////opaque

	//BeginRenderPass(cmd, m_RenderPasses[1], opaqueFramebuffer, { depthClearColor ,textureClearColor });
	//BeginPipeline(cmd, opaquePipeline);
	//for (size_t i = 0; i < opaqueModels.size(); i++)
	//{
	//	DrawMesh(cmd, opaqueModels[i], opaquePipeline, &opaqueDescriptorSet[i]);
	//}
	//vkCmdEndRenderPass(cmd);
	//shadow

	TransitionImageLayout(cmd, shadowDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	BeginRenderPass(cmd, shadowPass, shadow_Frambuffer, { depthClearColor }, { 4096,4096 });
	BeginPipeline(cmd, shadowPipeline);
	VkViewport viewPort = {
		0,0,4096,4096,0,1
	};
	VkRect2D scissor = {
		{0,0},{4096,4096}
	};
	vkCmdSetScissor(cmd, 0, 1, &scissor);
	vkCmdSetViewport(cmd, 0, 1, &viewPort);
	for (size_t i = 0; i < shadowModels.size(); i++)
	{
		DrawMesh(cmd, shadowModels[i], shadowPipeline, &shadowSets[i]);
	}
	vkCmdEndRenderPass(cmd);
	//differ
	//pass one
	BeginRenderPass(cmd, m_RenderPasses[2], differ_framebuffer[0], { depthClearColor ,textureClearColor,textureClearColor,textureClearColor }, m_SwapChainExtent);
	BeginPipeline(cmd, differPipelines[0]);
	 viewPort = {
		0,0,1024,768,0,1
	};
	 scissor = {
		{0,0},{1024,768}
	};
	vkCmdSetScissor(cmd, 0, 1, &scissor);
	vkCmdSetViewport(cmd, 0, 1, &viewPort);
	for (size_t i = 0; i < differModels.size(); i++)
	{
		DrawMesh(cmd, differModels[i], differPipelines[0], &differDescriptorSets[i]);
	}
	vkCmdEndRenderPass(cmd);
	shadowDepth.m_CurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	TransitionImageLayout(cmd, shadowDepth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
	//pass two
	BeginRenderPass(cmd, m_RenderPasses[3], differ_framebuffer[1], { depthClearColor ,textureClearColor }, m_SwapChainExtent);
	viewPort = {
		0,0,1024,768,0,1
	};

	vkCmdSetViewport(cmd, 0, 1, &viewPort);

	scissor = {
		{0,0},{1024,768}
	};
	vkCmdSetScissor(cmd, 0, 1, &scissor);
	BeginPipeline(cmd, differPipelines[1]);
	DrawMesh(cmd,full_screen_rect, differPipelines[1], &differ_secondStageDescriptorSet);
	vkCmdEndRenderPass(cmd);
	//SSR

	//BeginRenderPass(cmd, m_RenderPasses[4], SSR_framebuffer, { depthClearColor ,textureClearColor });

	//BeginPipeline(cmd, SSRPipelines);

	//for (size_t i = 0; i < SSRModels.size(); i++)
	//{
	//	DrawMesh(cmd, SSRModels[i], SSRPipelines, &SSRDescriptorSets[i]);
	//}
	//vkCmdEndRenderPass(cmd);




	DrawFrame(cmd, m_RenderTarget);
}


void VulkanApplication::SetResizable(bool state) {


}

bool Familys::isComplete(unsigned int m_QueueFlags) {

	bool pre = presentFamily.has_value();
	bool com = !(m_QueueFlags & QUEUE_COMPUTE_BIT) || computeFamily.has_value();
	bool gra = !(m_QueueFlags & QUEUE_GRAPHICS_BIT) || graphicsFamily.has_value();
	bool tra = !(m_QueueFlags & QUEUE_TRANSFER_BIT) || transferFamily.has_value();
	bool spar = !(m_QueueFlags & QUEUE_SPARSE_BINDING_BIT) || sparseBindingFamily.has_value();
	return pre && com && gra && tra && spar;
}

//help functions

void VulkanApplication::SetApplicationInfo(std::string applicationName, int width, int height) {
	m_ApplicationName = applicationName;
	m_Width = width;
	m_Height = height;
}


void VulkanApplication::SetRequiredValadationLayers(std::vector<const char*> valadationLayers) {
	m_ValidationLayers = valadationLayers;
}
void VulkanApplication::SetRequiredExtentions(std::vector<const char*> extentions) {
	m_RequiredExtentions = extentions;
}

void VulkanApplication::SetWindowHandle(HWND window, HINSTANCE handle) {
	m_HWND = window;
	m_HINSTANCE = handle;
}

void VulkanApplication::SetDeviceExtensions(std::vector<const char*> extentions) {
	m_DeviceExtensions = extentions;
}
void VulkanApplication::SetQueueFlag(QueueFlagBits flag) {
	m_QueueFlags = flag;
}

void VulkanApplication::CreateMyWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_Window = glfwCreateWindow(m_Width, m_Height, m_ApplicationName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
}


void VulkanApplication::CreateInstance() {

	if (m_EnableValidationLayer && !CheckValidationLayerSupport())
	{
		throw std::exception("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = m_ApplicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VoidEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (m_RequiredExtentions.size() == 0)
	{
		m_RequiredExtentions = GetRequiredExtensions();
	}
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_RequiredExtentions.size());
	createInfo.ppEnabledExtensionNames = m_RequiredExtentions.data();
	createInfo.enabledLayerCount = 0;
	if (m_EnableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
		throw std::exception("failed to create instance!");
	}
}

bool VulkanApplication::CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_ValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;

}

std::vector<const char*> VulkanApplication::GetRequiredExtensions() {
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

	if (m_EnableValidationLayer) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanApplication::CreateDebugCallback() {

	if (!m_EnableValidationLayer) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	if (m_DebugCallback != nullptr)
	{
		createInfo.pfnUserCallback = m_DebugCallback;

	}
	else
	{
		createInfo.pfnUserCallback = debugCallback;
	}
	createInfo.pUserData = nullptr; // Option
	VkResult r = CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);

	if (r != VK_SUCCESS) {
		throw std::exception("failed to set up debug call back");
	}
}

void VulkanApplication::CreateSurface() {
	if (m_HWND != nullptr)
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = m_HWND;
		createInfo.hinstance = m_HINSTANCE;

		if (vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface) != VK_SUCCESS) {
			throw std::exception("failed to create window surface!");
		}
	}
	else
	{
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS) {
			throw std::exception("failed to create window surface!");
		}
	}
}
void VulkanApplication::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::exception("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE) {
		throw std::exception("failed to find a suitable GPU!");
	}

}

bool VulkanApplication::IsDeviceSuitable(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		return false;
	}
	Familys indices = FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete(m_QueueFlags) && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;

}

Familys VulkanApplication::FindQueueFamilies(VkPhysicalDevice device) {
	Familys indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (size_t i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueCount > 0 && (m_QueueFlags & QUEUE_GRAPHICS_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		if (queueFamilies[i].queueCount > 0 && (m_QueueFlags & QUEUE_COMPUTE_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			indices.computeFamily = i;
		}
		if (queueFamilies[i].queueCount > 0 && (m_QueueFlags & QUEUE_TRANSFER_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			indices.transferFamily = i;
		}
		if (queueFamilies[i].queueCount > 0 && (m_QueueFlags & QUEUE_SPARSE_BINDING_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			indices.sparseBindingFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}
		if (indices.isComplete(m_QueueFlags))
		{
			return indices;
		}
	}

	return indices;
}

SwapChainSupportDetails VulkanApplication::QuerySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool VulkanApplication::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void VulkanApplication::CreateDevice() {

	Familys indices = FindQueueFamilies(m_PhysicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;
	deviceFeatures.shaderClipDistance = VK_TRUE;
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (m_EnableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicDevice) != VK_SUCCESS) {
		throw std::exception("failed to create logical device!");
	}

	GetQueues(indices);

}

void VulkanApplication::GetQueues(Familys queueFamily) {
	if (m_QueueFlags & QUEUE_GRAPHICS_BIT)
	{
		vkGetDeviceQueue(m_LogicDevice, queueFamily.graphicsFamily.value(), 0, &m_Queues.graphicsQueue);
	}

	if (m_QueueFlags & QUEUE_COMPUTE_BIT)
	{
		vkGetDeviceQueue(m_LogicDevice, queueFamily.computeFamily.value(), 0, &m_Queues.computeQueue);
	}

	if (m_QueueFlags & QUEUE_TRANSFER_BIT)
	{
		vkGetDeviceQueue(m_LogicDevice, queueFamily.transferFamily.value(), 0, &m_Queues.transferQueue);
	}

	if (m_QueueFlags & QUEUE_SPARSE_BINDING_BIT)
	{
		vkGetDeviceQueue(m_LogicDevice, queueFamily.sparseBindingFamily.value(), 0, &m_Queues.sparseBindingQueue);
	}

	vkGetDeviceQueue(m_LogicDevice, queueFamily.presentFamily.value(), 0, &m_Queues.presentQueue);
}

void VulkanApplication::CreateSwapChain() {
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	Familys indices = FindQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	VkResult r = vkCreateSwapchainKHR(m_LogicDevice, &createInfo, nullptr, &m_SwapChain);
	if (r != VK_SUCCESS) {
		throw std::exception("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_LogicDevice, m_SwapChain, &imageCount, nullptr);
	std::vector<VkImage> images(imageCount);
	m_SwapChainImages.resize(imageCount);

	vkGetSwapchainImagesKHR(m_LogicDevice, m_SwapChain, &imageCount, images.data());
	for (size_t i = 0; i < images.size(); ++i)
	{
		m_SwapChainImages[i].m_Image = images[i];
	}
	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}


VkSurfaceFormatKHR VulkanApplication::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}


VkPresentModeKHR VulkanApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		return actualExtent;
	}

}


void VulkanApplication::CreateSwapChainImageViews() {

	for (uint32_t i = 0; i < m_SwapChainImages.size(); i++) {
		//TransitionImageLayout(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat);
	}
}


#pragma region help functions
Buffer VulkanApplication::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	Buffer buffer;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_LogicDevice, &bufferInfo, nullptr, &buffer.m_Buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements m_emRequirements;
	vkGetBufferMemoryRequirements(m_LogicDevice, buffer.m_Buffer, &m_emRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = m_emRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(m_emRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_LogicDevice, &allocInfo, nullptr, &buffer.m_Memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_LogicDevice, buffer.m_Buffer, buffer.m_Memory, 0);
	buffer.m_Size = size;
	buffer.m_BufferInfo.buffer = buffer.m_Buffer;
	buffer.m_BufferInfo.offset = 0;
	buffer.m_BufferInfo.range = buffer.m_Size;
	return buffer;
}

void VulkanApplication::MapBuffer(Buffer& buffer) {
	vkMapMemory(m_LogicDevice, buffer.m_Memory, 0, buffer.m_Size, 0, &buffer.m_Data);
}

void VulkanApplication::CopyDataToBuffer(void* data, Buffer& buffer, uint32_t size) {
	if (size == 0)
	{
		size = buffer.m_Size;
	}
	memcpy(buffer.m_Data, data, size);
}

void VulkanApplication::UnMapBuffer(Buffer& buffer) {
	vkUnmapMemory(m_LogicDevice, buffer.m_Memory);
}

Image VulkanApplication::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageType type, uint32_t layers, VkImageTiling tiling) {
	Image image = {};
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = type;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = layers;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (layers == 6)
	{
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}

	if (vkCreateImage(m_LogicDevice, &imageInfo, nullptr, &image.m_Image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements m_emRequirements;
	vkGetImageMemoryRequirements(m_LogicDevice, image.m_Image, &m_emRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = m_emRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(m_emRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_LogicDevice, &allocInfo, nullptr, &image.m_Memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(m_LogicDevice, image.m_Image, image.m_Memory, 0);
	image.m_Format = format;
	SetDescriptorImageInfo(image);
	return image;
}

void VulkanApplication::CreateImageView(Image& image, VkFormat format, VkImageAspectFlags flag, VkImageViewType type) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image.m_Image;
	viewInfo.viewType = type;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = flag;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	if (type == VK_IMAGE_VIEW_TYPE_CUBE)
	{
		viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		// 6 array layers (faces)
		viewInfo.subresourceRange.layerCount = 6;
		// Set number of mip levels
		viewInfo.subresourceRange.levelCount = 1;
	}
	if (vkCreateImageView(m_LogicDevice, &viewInfo, nullptr, &image.m_ImageView) != VK_SUCCESS) {
		throw std::exception("failed to create texture image view!");
	}
}


VkFramebuffer VulkanApplication::CreateFrameBuffer(std::vector<Image> images, VkRenderPass renderPass,uint32_t width,uint32_t height) {
	std::vector<VkImageView> attachments(images.size());
	size_t i = 0;
	for (Image& image : images)
	{
		attachments[i] = image.m_ImageView;
		++i;
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;
	VkFramebuffer frameBuffer;
	if (vkCreateFramebuffer(m_LogicDevice, &framebufferInfo, nullptr, &frameBuffer) != VK_SUCCESS) {
		throw std::exception("failed to create framebuffer!");
	}
	return frameBuffer;
}


void VulkanApplication::CopyBufferToBuffer(VkCommandBuffer cmd, Buffer srcBuffer, Buffer dstBuffer) {

	VkBufferCopy copyRegion = {};
	copyRegion.size = srcBuffer.m_Size;
	vkCmdCopyBuffer(cmd, srcBuffer.m_Buffer, dstBuffer.m_Buffer, 1, &copyRegion);

}


void VulkanApplication::CopyBufferToImage(VkCommandBuffer cmd, uint32_t width, uint32_t height, Buffer srcBuffer, Image& dstImage, std::vector<VkBufferImageCopy> region) {
	if (region.size() == 0)
	{
		VkBufferImageCopy rr = {};
		rr.bufferOffset = 0;
		rr.bufferRowLength = 0;
		rr.bufferImageHeight = 0;

		rr.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		rr.imageSubresource.mipLevel = 0;
		rr.imageSubresource.baseArrayLayer = 0;
		rr.imageSubresource.layerCount = 1;

		rr.imageOffset = { 0, 0, 0 };
		rr.imageExtent = {
			width,
			height,
			1
		};
		region.push_back(rr);
	}
	vkCmdCopyBufferToImage(
		cmd,
		srcBuffer.m_Buffer,
		dstImage.m_Image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		region.size(),
		region.data()
	);
}
void VulkanApplication::CopyImageToImage(VkCommandBuffer cmd, uint32_t width, uint32_t height, Image& srcImage, Image& dstImage) {
	VkImageLayout srcOld = srcImage.m_CurrentLayout;;
	VkImageLayout dstOld = dstImage.m_CurrentLayout;
	TransitionImageLayout(cmd, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	TransitionImageLayout(cmd, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkImageCopy copy = {};
	copy.dstOffset = {};
	copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.dstSubresource.baseArrayLayer = 0;
	copy.dstSubresource.layerCount = 1;
	copy.dstSubresource.mipLevel = 0;
	copy.srcOffset = {};
	copy.srcSubresource = copy.dstSubresource;
	copy.extent = { width,height,0 };
	vkCmdCopyImage(cmd, srcImage.m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
	TransitionImageLayout(cmd, srcImage, srcOld);
	TransitionImageLayout(cmd, dstImage, dstOld);
}

void VulkanApplication::TransitionImageLayout(VkCommandBuffer cmd, Image& image, VkImageLayout newLayout, VkImageSubresourceRange subSourceRange) {

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = image.m_CurrentLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image.m_Image;
	barrier.subresourceRange = subSourceRange;
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{

		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{

		barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	
	}

	else if (image.m_CurrentLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
	{
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

	barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
	sourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		cmd,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	image.m_CurrentLayout = newLayout;
}

VkAttachmentDescription VulkanApplication::CreateAttachmentDescriptionForColorAttachment(VkFormat format,
	VkImageLayout startLayout,
	VkImageLayout endLayout,
	VkAttachmentLoadOp loadOp,
	VkAttachmentStoreOp storeOp) {
	VkAttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = loadOp;
	attachment.storeOp = storeOp;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = startLayout;
	attachment.finalLayout = endLayout;
	return attachment;
}



VkAttachmentDescription VulkanApplication::CreateAttachmentDescriptionForDepthAttachment(VkFormat format,
	VkAttachmentLoadOp loadOp,//= VK_ATTACHMENT_LOAD_OP_CLEAR,
	VkAttachmentStoreOp storeOp,// = VK_ATTACHMENT_STORE_OP_STORE,
	VkImageLayout startLayout,//= VK_IMAGE_LAYOUT_UNDEFINED,
	VkImageLayout endLayout,// = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	VkAttachmentLoadOp stencilLoadOp,// = VK_ATTACHMENT_LOAD_OP_CLEAR,
	VkAttachmentStoreOp stencilStoreOp//= VK_ATTACHMENT_STORE_OP_STORE
) {

	VkAttachmentDescription attachment = {};
	attachment.format = format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = loadOp;
	attachment.storeOp = storeOp;
	attachment.stencilLoadOp = stencilLoadOp;
	attachment.stencilStoreOp = stencilStoreOp;
	attachment.initialLayout = startLayout;
	attachment.finalLayout = endLayout;
	return attachment;


}


VkAttachmentReference CreateAttachmentReference(uint32_t attachint, VkImageLayout layout) {
	VkAttachmentReference refe = {};
	refe.attachment = attachint;
	refe.layout = layout;
	return refe;
}

void  VulkanApplication::SetSubpassDescriptionColorAttachment(VkSubpassDescription& description, VkAttachmentReference* attachmentsRef, uint32_t size) {

	description.colorAttachmentCount = size;
	description.pColorAttachments = attachmentsRef;
}
void  VulkanApplication::SetSubpassDescriptionDepthAttachment(VkSubpassDescription& description, const VkAttachmentReference& attachmentRef) {

	description.pDepthStencilAttachment = &attachmentRef;
}
void  VulkanApplication::SetSubpassDescriptionDepthAttachment(VkSubpassDescription& description, VkAttachmentReference* attachmentsRef, uint32_t size) {
	description.inputAttachmentCount = size;
	description.pInputAttachments = attachmentsRef;
}
void  VulkanApplication::SetSubpassDescriptionBindPoint(VkSubpassDescription& description, VkPipelineBindPoint bindpoint) {
	description.pipelineBindPoint = bindpoint;
}


VkSubpassDependency VulkanApplication::CreateSubpassDependency(uint32_t srcSubpass,
	uint32_t                dstSubpass,
	VkPipelineStageFlags    srcStageMask,
	VkPipelineStageFlags    dstStageMask,
	VkAccessFlags           srcAccessMask,
	VkAccessFlags           dstAccessMask,
	VkDependencyFlags       dependencyFlags) {
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = srcSubpass;
	dependency.dstSubpass = dstSubpass;
	dependency.srcStageMask = srcStageMask;
	dependency.dstStageMask = dstStageMask;
	dependency.srcAccessMask = srcAccessMask;
	dependency.dstAccessMask = dstAccessMask;
	dependency.dependencyFlags = dependencyFlags;
	return dependency;
}
VkRenderPass VulkanApplication::CreateRenderPass(std::vector< VkAttachmentDescription> attachments, std::vector< VkSubpassDescription> subpasses, std::vector<VkSubpassDependency> dependencies) {

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();
	VkRenderPass m_RenderPass;
	if (vkCreateRenderPass(m_LogicDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
		throw std::exception("failed to create render pass!");
	}
	return m_RenderPass;
}


VkPipelineShaderStageCreateInfo VulkanApplication::CreateShader(std::string name, VkShaderStageFlagBits stage, const char* enterFunction) {
	std::filesystem::path path();
	std::string fullpath = "..\\shader\\" + name + ".spv";
#ifdef _DEBUG
	std::string callPath = "..\\shader\\compile.bat ..\\shader\\" + name;
	system(callPath.c_str());
#endif // COMPILE_SHADER	
	auto code = readFile(fullpath);
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	VkShaderModule shaderModule;

	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	if (vkCreateShaderModule(m_LogicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::exception("failed to create shader module!");
	}
	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = stage;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = enterFunction;

	return shaderStageInfo;
}

Pipeline VulkanApplication::CreatePipeline(VkRenderPass renderPass,
	std::vector<VkPipelineShaderStageCreateInfo> stages,
	std::vector<VkVertexInputBindingDescription> bindings,
	std::vector<VkVertexInputAttributeDescription> attributes,
	std::vector<VkDescriptorSetLayout> laytous,
	VkPipelineDepthStencilStateCreateInfo depthInfo,
	uint32_t subpass,
	std::vector < VkPipelineColorBlendStateCreateInfo> colorBlendings,
	std::vector<VkViewport> viewports,
	std::vector <VkRect2D> scissors,
	std::vector < VkPipelineRasterizationStateCreateInfo> rasterizers,
	std::vector < VkPipelineMultisampleStateCreateInfo> multisamplings,
	std::vector < VkDynamicState> dynamicStates
) {
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount = bindings.size();
	vertexInputInfo.vertexAttributeDescriptionCount = attributes.size();
	vertexInputInfo.pVertexBindingDescriptions = bindings.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkExtent2D swipChainExtent = m_SwapChainExtent;

	if (viewports.size() == 0)
	{
		VkViewport m_viewport = {};
		m_viewport.x = 0;
		m_viewport.y = 0;
		m_viewport.width = swipChainExtent.width;
		m_viewport.height = swipChainExtent.height;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;
		viewports.push_back(m_viewport);
	}

	if (scissors.size() == 0)
	{
		VkRect2D m_scissor = {};
		m_scissor.offset = { 0,0 };
		m_scissor.extent = swipChainExtent;
		scissors.push_back(m_scissor);
	}

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = viewports.size();
	viewportState.pViewports = viewports.data();
	viewportState.scissorCount = scissors.size();
	viewportState.pScissors = scissors.data();
	if (rasterizers.size() == 0)
	{
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizers.push_back(rasterizer);
	}

	if (multisamplings.size() == 0)
	{
		VkPipelineMultisampleStateCreateInfo m_ultisampling = {};
		m_ultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_ultisampling.sampleShadingEnable = VK_FALSE;
		m_ultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_ultisampling.minSampleShading = 1.0f; // Optional
		m_ultisampling.pSampleMask = nullptr; // Optional
		m_ultisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		m_ultisampling.alphaToOneEnable = VK_FALSE; // Optional
		multisamplings.push_back(m_ultisampling);
	}


	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	if (colorBlendings.size() == 0)
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachments.push_back(colorBlendAttachment);

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
		colorBlendings.push_back(colorBlending);
	}

	VkDynamicState dynamicStates1[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount =2;
	dynamicState.pDynamicStates = dynamicStates1;

	//VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	//layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	//layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	//layoutInfo.bindingCount = layoutBindings.size();
	//layoutInfo.pBindings = layoutBindings.data();
	//if (vkCreateDescriptorSetLayout(m_LogicDevice, &layoutInfo, nullptr, laytous.data()) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to create descriptor set layout!");
	//}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = laytous.size(); // Optional
	pipelineLayoutInfo.pSetLayouts = laytous.data(); // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = 0; // Optional
	Pipeline pipeline = {};

	if (vkCreatePipelineLayout(m_LogicDevice, &pipelineLayoutInfo, nullptr, &pipeline.m_Layout) != VK_SUCCESS) {
		throw std::exception("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = stages.size();
	pipelineInfo.pStages = stages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = rasterizers.data();
	pipelineInfo.pMultisampleState = multisamplings.data();
	pipelineInfo.pDepthStencilState = nullptr; // Optional

	pipelineInfo.pDepthStencilState = &depthInfo;
	
	pipelineInfo.pColorBlendState = colorBlendings.data();
	pipelineInfo.pDynamicState = &dynamicState; // Optional
	pipelineInfo.layout = pipeline.m_Layout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = subpass;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkResult r = vkCreateGraphicsPipelines(m_LogicDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.m_Pipeline);
	if (r != VK_SUCCESS) {
		throw std::exception("failed to create graphics pipeline!");
	}

	//m_RenderPass_Pipelines[renderPass].push_back(pipeline);
	for (auto& stage : stages)
	{
		vkDestroyShaderModule(m_LogicDevice, stage.module, nullptr);
	}
	return pipeline;
}

VkCommandBuffer VulkanApplication::CreateCommandBuffer(bool start) {
	VkCommandBuffer command;
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_LogicDevice, &allocInfo, &command) != VK_SUCCESS) {
		throw std::exception("failed to allocate command buffers!");
	}

	if (start)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		vkBeginCommandBuffer(command, &beginInfo);
	}
	return command;
}


void VulkanApplication::DestroyBuffer(Buffer& buffer) {
	vkDestroyBuffer(m_LogicDevice, buffer.m_Buffer, nullptr);
	vkFreeMemory(m_LogicDevice, buffer.m_Memory, nullptr);
}

VkDescriptorSetLayoutBinding VulkanApplication::CreateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType,
	VkShaderStageFlags stageFlags, uint32_t descriptorCount) {

	VkDescriptorSetLayoutBinding bind = {};
	bind.binding = binding;
	bind.descriptorCount = descriptorCount;
	bind.descriptorType = descriptorType;
	bind.pImmutableSamplers = nullptr;
	bind.stageFlags = stageFlags;
	return bind;

}

VkDescriptorSetLayout VulkanApplication::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings) {
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	VkDescriptorSetLayout descriptorSetLayout;
	if (vkCreateDescriptorSetLayout(m_LogicDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	return descriptorSetLayout;
}

VkPipelineColorBlendAttachmentState VulkanApplication::CreatePipelineColorBlendAttachmentState(VkBool32 blendEnable,
	VkBlendFactor            srcColorBlendFactor,
	VkBlendFactor            dstColorBlendFactor,
	VkBlendOp                colorBlendOp,
	VkBlendFactor            srcAlphaBlendFactor,
	VkBlendFactor            dstAlphaBlendFactor,
	VkBlendOp                alphaBlendOp,
	VkColorComponentFlags    colorWriteMask) {
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = colorWriteMask;
	colorBlendAttachment.blendEnable = blendEnable;
	colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor; // Optional
	colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor; // Optional
	colorBlendAttachment.colorBlendOp = colorBlendOp; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor; // Optional
	colorBlendAttachment.alphaBlendOp = alphaBlendOp; // Optional
	
	return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo VulkanApplication::CreatePipelineColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState* colorBlendAttachments, size_t size,
	VkBool32 logicOpEnable, VkLogicOp logicOp
) {

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = logicOpEnable;
	colorBlending.logicOp = logicOp; // Optional
	colorBlending.attachmentCount = size;
	colorBlending.pAttachments = colorBlendAttachments;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
	return colorBlending;

}



VkPipelineDepthStencilStateCreateInfo VulkanApplication::CreateDepthStencilStateCreateInfo(
	VkBool32                                  depthTestEnable,
	VkBool32                                  depthWriteEnable ,
	VkCompareOp                               depthCompareOp,
	float                                     minDepthBounds,
	float                                     maxDepthBounds,
	VkBool32                                  depthBoundsTestEnable,
	VkBool32                                  stencilTestEnable,
	VkStencilOpState                          front,
	VkStencilOpState                          back,
	VkPipelineDepthStencilStateCreateFlags    flags) {
	VkPipelineDepthStencilStateCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	info.depthTestEnable = depthTestEnable;
	info.depthWriteEnable = depthWriteEnable;
	info.depthCompareOp = depthCompareOp;
	info.depthBoundsTestEnable = depthBoundsTestEnable;
	info.stencilTestEnable = stencilTestEnable;
	info.front = front;
	info.back = back;
	info.minDepthBounds = minDepthBounds;
	info.maxDepthBounds = maxDepthBounds;
	return info;
}



#pragma endregion
#pragma region pipeline example
//void VulkanApplication::CreateGraphicsPipeline() {
//
//	//Pipeline one
//	{
//		system("..\\shader\\compile.bat skybox");
//		{
//			Shader verte("..\\shader\\vert.spv", ShaderStage::SHADER_STAGE_VERTEX_BIT);
//			Shader frag("..\\shader\\frag.spv", ShaderStage::SHADER_STAGE_FRAGMENT_BIT);
//			std::vector<Shader> shaders = { verte,frag };
//
//			PipelineBindingInfo binding{ 0,sizeof(Vertex_Aki),INPUT_RATE_VERTEX };
//			std::vector<PipelineBindingInfo>bindingInfos = { binding };
//
//			std::vector<PipelineAttributeInfo> attributeDescripti(6);
//
//			attributeDescripti[0].binding = 0;
//			attributeDescripti[0].location = 0;
//			attributeDescripti[0].format = FORMAT_R32G32B32_SFLOAT;
//			attributeDescripti[0].offset = offsetof(Vertex_Aki, position);
//
//			attributeDescripti[1].binding = 0;
//			attributeDescripti[1].location = 1;
//			attributeDescripti[1].format = FORMAT_R32G32B32_SFLOAT;
//			attributeDescripti[1].offset = offsetof(Vertex_Aki, normal);
//			
//
//			attributeDescripti[2].binding = 0;
//			attributeDescripti[2].location = 2;
//			attributeDescripti[2].format = FORMAT_R32G32B32_SFLOAT;
//			attributeDescripti[2].offset = offsetof(Vertex_Aki, tangent);
//
//
//			attributeDescripti[3].binding = 0;
//			attributeDescripti[3].location = 3;
//			attributeDescripti[3].format = FORMAT_R32G32_SFLOAT;
//			attributeDescripti[3].offset = offsetof(Vertex_Aki, texCoord);
//
//			attributeDescripti[4].binding = 0;
//			attributeDescripti[4].location = 4;
//			attributeDescripti[4].format = FORMAT_R32G32B32_SFLOAT;
//			attributeDescripti[4].offset = offsetof(Vertex_Aki, texCoord2);
//
//			attributeDescripti[5].binding = 0;
//			attributeDescripti[5].location = 5;
//			attributeDescripti[5].format = FORMAT_R32G32B32A32_SFLOAT;
//			attributeDescripti[5].offset = offsetof(Vertex_Aki, texCoord3);
//
//			std::vector<PipelineDescriptorSetInfo> desInfo(3);
//			desInfo[0].binding = 0;
//			desInfo[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;//cameraInfo
//			desInfo[0].count = 1;
//			desInfo[0].stageFlags = SHADER_STAGE_VERTEX_BIT;
//
//			desInfo[1].binding = 1;
//			desInfo[1].type = DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//cube m_ap
//			desInfo[1].count = 1;
//			desInfo[1].stageFlags = SHADER_STAGE_FRAGMENT_BIT;
//		
//			desInfo[2].binding = 2;
//			desInfo[2].type = DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//texture m_ap
//			desInfo[2].count = 1;
//			desInfo[2].stageFlags = SHADER_STAGE_FRAGMENT_BIT;
//			Pipeline newPipeline({ verte,frag }, { binding }, attributeDescripti, desInfo, TOPOLOGY_TRIANGLE_LIST, Rect{ 0,0,800,800 }, IntRect{ 0,0,800,800 });
//
//			m_PipelineLayouts[m_RenderPass].push_back(newPipeline.m_PipelineLayout);
//			mPipelines[m_RenderPass].push_back(newPipeline.m_Pipeline);
//
//			for (int i = 0; i < shaders.size(); i++)
//			{
//				shaders[i].Destroy();
//			}
//		}
//	}
//}
#pragma endregion

void VulkanApplication::CreateCommandPool() {
	Familys queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional
	if (vkCreateCommandPool(m_LogicDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::exception("failed to create command pool!");
	}
}

void VulkanApplication::CreateSyncObjects() {
	m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(m_LogicDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_LogicDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_LogicDevice, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {

			throw std::exception("failed to create synchronization objects for a frame!");
		}
	}
}

void VulkanApplication::RecreateSwapChain() {
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(m_Window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_LogicDevice);
	CleanupSwapChain();
	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateDescriptorPool();

}


void VulkanApplication::UpdateUniformBuffer() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	camera.viewInfo.position = pos;
	camera.viewInfo.view = glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f));

	camera.viewInfo.proj = glm::perspective(glm::radians(fov), m_SwapChainExtent.width / (float)m_SwapChainExtent.height, camera.nearClip, camera.farClip);
	//camera.viewInfo.proj = glm::ortho(-512.0f, 512.0f,-384.0f,384.0f, 0.01f, 1000.0f);

	camera.viewInfo.proj[1][1] *= -1;
	CopyDataToBuffer(&camera.viewInfo, m_CameraBuffer);
	//vkUnmapMemory(m_LogicDevice, m_CameraBuffer.m_Memory);s
}

Image VulkanApplication::CreateTextureImage(const char* filePath) {
	int x, y, c;
	void* texdata = stbi_load(filePath, &x, &y, &c, STBI_rgb_alpha);
	Image image = CreateImage(x, y, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	Buffer stagingBuffer = CreateBuffer(x * y * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	void* data;
	vkMapMemory(m_LogicDevice, stagingBuffer.m_Memory, 0, stagingBuffer.m_Size, 0, &data);
	memcpy(data, texdata, x * y * 4);
	vkUnmapMemory(m_LogicDevice, stagingBuffer.m_Memory);
	VkCommandBuffer cmd = CreateCommandBuffer(true);
	TransitionImageLayout(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(cmd, x, y, stagingBuffer, image, {});
	TransitionImageLayout(cmd, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	CreateImageView(image, VK_FORMAT_R8G8B8A8_UNORM);
	EndSingleTimeCommands(cmd);
	SetSampler(image, m_TextureSampler);
	SetDescriptorImageInfo(image);
	return image;
}

void VulkanApplication::SetDescriptorImageInfo(Image& image, VkImageLayout layout) {
	image.m_ImageInfo.imageLayout = layout;
	image.m_ImageInfo.sampler = image.m_Sampler;
	image.m_ImageInfo.imageView = image.m_ImageView;
}

Image VulkanApplication::CreateCubeMap(std::vector<std::string> files) {

	int texWidth, texHeight, texChannels;
	assert(files.size() == 6);
	stbi_uc* pixels = stbi_load(files[0].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	Image cube = CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TYPE_2D, 6, VK_IMAGE_TILING_OPTIMAL);

	// Create a host-visible staging buffer that contains the raw image data
	Buffer stagingBuffer = CreateBuffer(texWidth * texHeight * 4 * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkMemoryAllocateInfo m_emAllocInfo;

	void* data;

	uint32_t offset = 0;
	std::vector<VkBufferImageCopy> bufferCopyRegions;
	for (size_t i = 0; i < 6; i++)
	{
		stbi_uc* pixels = stbi_load(files[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;
		VkBufferImageCopy bufferCopyRegion = {};
		bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferCopyRegion.imageSubresource.mipLevel = 0;
		bufferCopyRegion.imageSubresource.baseArrayLayer = i;
		bufferCopyRegion.imageSubresource.layerCount = 1;
		bufferCopyRegion.imageExtent.width = texWidth;
		bufferCopyRegion.imageExtent.height = texHeight;
		bufferCopyRegion.imageExtent.depth = 1;
		bufferCopyRegion.bufferOffset = offset;
		VkResult r = vkMapMemory(m_LogicDevice, stagingBuffer.m_Memory, offset, imageSize, 0, &data);
		memcpy(data, pixels, imageSize);
		vkUnmapMemory(m_LogicDevice, stagingBuffer.m_Memory);
		bufferCopyRegions.push_back(bufferCopyRegion);
		offset += texWidth * texHeight * 4;
	}
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 6;
	VkCommandBuffer cmd = CreateCommandBuffer();
	TransitionImageLayout(cmd, cube, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
	CopyBufferToImage(cmd, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), stagingBuffer, cube, bufferCopyRegions);
	TransitionImageLayout(cmd, cube, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
	EndSingleTimeCommands(cmd);
	CreateImageView(cube, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
	SetDescriptorImageInfo(cube);
	return cube;
}

VkSampler VulkanApplication::CreateTextureSampler() {

	VkSampler sap;
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;


	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_NEVER;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler(m_LogicDevice, &samplerInfo, nullptr, &sap) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
	return sap;

}

void VulkanApplication::SetSampler(Image& image, VkSampler sampler) {
	image.m_Sampler = sampler;
	image.m_ImageInfo.sampler = sampler;
}



uint32_t VulkanApplication::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::exception("failed to find suitable memory type!");
	return -1;
}

VkCommandBuffer VulkanApplication::BeginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_LogicDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanApplication::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	vkCreateFence(m_LogicDevice, &fenceInfo, nullptr, &fence);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_Queues.graphicsQueue, 1, &submitInfo, fence);
	vkWaitForFences(m_LogicDevice, 1, &fence, VK_TRUE, 100000000000);

	//vkQueueWaitIdle(m_Queues.graphicsQueue);
	vkDestroyFence(m_LogicDevice, fence, nullptr);
	vkFreeCommandBuffers(m_LogicDevice, m_CommandPool, 1, &commandBuffer);
}

Buffer VulkanApplication::CreateVertexBuffer(std::vector<Vertex_Aki>vertexdata) {

	VkDeviceSize bufferSize = sizeof(vertexdata[0]) * vertexdata.size();
	Buffer stagingBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(m_LogicDevice, stagingBuffer.m_Memory, 0, bufferSize, 0, &data);
	memcpy(data, vertexdata.data(), (size_t)bufferSize);
	vkUnmapMemory(m_LogicDevice, stagingBuffer.m_Memory);

	Buffer vertexbuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd = CreateCommandBuffer();
	CopyBufferToBuffer(cmd, stagingBuffer, vertexbuffer);
	EndSingleTimeCommands(cmd);
	DestroyBuffer(stagingBuffer);
	return vertexbuffer;
}
Buffer VulkanApplication::CreateIndexBuffer(std::vector<uint32_t>indices) {
	VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();;
	/*if (indices.size() < 65535)
	{
		bufferSize = sizeof(uint16_t) * indices.size();
	}
	else
	{
		bufferSize = sizeof(uint32_t) * indices.size();
	}*/

	Buffer stagingBuffer = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(m_LogicDevice, stagingBuffer.m_Memory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), bufferSize);
	vkUnmapMemory(m_LogicDevice, stagingBuffer.m_Memory);

	Buffer index = CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkCommandBuffer cmd = CreateCommandBuffer();
	CopyBufferToBuffer(cmd, stagingBuffer, index);
	EndSingleTimeCommands(cmd);
	DestroyBuffer(stagingBuffer);
	return index;
}

VkVertexInputBindingDescription VulkanApplication::CreateVertexInputBindingDescription(uint32_t binding, VkVertexInputRate inputRate, uint32_t stride) {
	VkVertexInputBindingDescription des = {};
	des.binding = binding;
	des.inputRate = inputRate;
	des.stride = stride;
	return des;
}

VkVertexInputAttributeDescription VulkanApplication::CreateVertexInputAttributeDescription(uint32_t  location, uint32_t binding, VkFormat format, uint32_t offset) {
	VkVertexInputAttributeDescription des = {};
	des.binding = binding;
	des.location = location;
	des.format = format;
	des.offset = offset;
	return des;
}
VkDescriptorSetAllocateInfo VulkanApplication::CreateDescriptorSetAllocateInfo(VkDescriptorSetLayout* descriptorSetLayouts, uint32_t count) {
	VkDescriptorSetAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	info.descriptorPool = m_DescriptorPool;
	info.descriptorSetCount = count;
	info.pSetLayouts = descriptorSetLayouts;
	return info;
}

void VulkanApplication::UpdateDescriptorSets(std::vector<VkWriteDescriptorSet> writeDescriptors) {

	vkUpdateDescriptorSets(m_LogicDevice, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
}

VkDescriptorSet VulkanApplication::CreateDescriptorSet(std::vector<VkDescriptorSetAllocateInfo> allocateInfo) {
	VkDescriptorSet descriptorSet;
	if (vkAllocateDescriptorSets(m_LogicDevice, allocateInfo.data(), &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	return descriptorSet;
}


Model VulkanApplication::UploadMesh(Mesh* mesh, int submesh) {

	mesh->mSubmeshes[submesh]->mVertexBuffer = CreateVertexBuffer(mesh->mSubmeshes[submesh]->mVertices);
	mesh->mSubmeshes[submesh]->mIndexBuffer = CreateIndexBuffer(mesh->mSubmeshes[submesh]->mIndicies);
	Model buffer = {};
	buffer = { mesh->mVertexBuffer,mesh->mIndexBuffer };
	buffer.m_IndexSize = mesh->mSubmeshes[submesh]->mIndicies.size();
	if (mesh->m_Path != nullptr)
	{
		mesh->m_Texture = CreateTextureImage(mesh->m_Path);
	}
	return buffer;
}


void VulkanApplication::UploadOpaqueMesh(Mesh* mesh, int submesh) {
	VkRenderPass pass = m_RenderPasses[1];

	mesh->mSubmeshes[submesh]->mVertexBuffer = CreateVertexBuffer(mesh->mSubmeshes[submesh]->mVertices);
	mesh->mSubmeshes[submesh]->mIndexBuffer = CreateIndexBuffer(mesh->mSubmeshes[submesh]->mIndicies);
	if (true)
	{
		Model mod = { mesh->mVertexBuffer,mesh->mIndexBuffer };
		mod.m_IndexSize = mesh->mSubmeshes[submesh]->mIndicies.size();

		//create descriptor set
		VkDescriptorSet opaqueset;

		if (mesh->m_Path != nullptr)
		{
			mod.m_Texture = CreateTextureImage(mesh->m_Path);
		}
		else
		{
			mod.m_Texture = m_WhiteImage;
		}

		//create descriptor set
		mod.m_Modeling = CreateBuffer(sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		MapBuffer(mod.m_Modeling);
		auto allocateinfo = CreateDescriptorSetAllocateInfo(&opaqueDescriptorSetLayout);
		opaqueset = CreateDescriptorSet({ allocateinfo });
		opaqueDescriptorSet.push_back(opaqueset);
		auto imageWrite = CreateWriteDescriptorSetForImage(mod.m_Texture, 2, opaqueset);
		auto cameraBuffer = CreateWriteDescriptorSetForBuffer(m_CameraBuffer, 0, opaqueset);
		auto modelBuffer = CreateWriteDescriptorSetForBuffer(mod.m_Modeling, 1, opaqueset);
		UpdateDescriptorSets({ imageWrite, cameraBuffer ,modelBuffer });
		opaqueModels.push_back(mod);
	}

}



void VulkanApplication::CreateDescriptorPool() {
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1000;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1000;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1000;

	if (vkCreateDescriptorPool(m_LogicDevice, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

}

VkWriteDescriptorSet VulkanApplication::CreateWriteDescriptorSetForBuffer(const Buffer& buffer, uint32_t binding, VkDescriptorSet descriptorSet) {

	VkWriteDescriptorSet descriptorWrites = {};
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstSet = descriptorSet;
	descriptorWrites.dstBinding = binding;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites.descriptorCount = 1;
	descriptorWrites.pBufferInfo = &buffer.m_BufferInfo;
	return descriptorWrites;
}
VkWriteDescriptorSet VulkanApplication::CreateWriteDescriptorSetForImage(const Image& image, uint32_t binding, VkDescriptorSet descriptorSet) {

	VkWriteDescriptorSet descriptorWrites = {};
	descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites.dstSet = descriptorSet;
	descriptorWrites.dstBinding = binding;
	descriptorWrites.dstArrayElement = 0;
	descriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites.descriptorCount = 1;
	descriptorWrites.pImageInfo = &image.m_ImageInfo;
	return descriptorWrites;
}

//void VulkanApplication::CreateDescriptorSets() {
//	
//	VkDescriptorSetAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfo.descriptorPool = m_DescriptorPool;
//	allocInfo.descriptorSetCount = 1;
//	allocInfo.pSetLayouts = &m_DescriptorSetLayout;
//	//TODO
//	if (vkAllocateDescriptorSets(m_LogicDevice, &allocInfo, &mDescriptorSet) != VK_SUCCESS) {
//		throw std::runtime_error("failed to allocate descriptor sets!");
//	}
//	//buffer info
//	VkDescriptorBufferInfo bufferInfo = {};
//	bufferInfo.buffer = m_CameraBuffer;
//	bufferInfo.offset = 0;
//	bufferInfo.range = sizeof(CameraViewInfo);
//
//	VkDescriptorImageInfo imageInfo = {};
//	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//	imageInfo.imageView = cubeMapImageView;
//	imageInfo.sampler = m_TextureSampler;
//
//	VkDescriptorImageInfo imageInfo2 = {};
//	imageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//	imageInfo2.imageView = m_TextureImageView;
//	imageInfo2.sampler = m_TextureSampler;
//
//
//	std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};
//
//	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//	descriptorWrites[0].dstSet = m_DescriptorSet;
//	descriptorWrites[0].dstBinding = 0;
//	descriptorWrites[0].dstArrayElement = 0;
//	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	descriptorWrites[0].descriptorCount = 1;
//	descriptorWrites[0].pBufferInfo = &bufferInfo;
//
//	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//	descriptorWrites[1].dstSet = m_DescriptorSet;
//	descriptorWrites[1].dstBinding = 1;
//	descriptorWrites[1].dstArrayElement = 0;
//	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	descriptorWrites[1].descriptorCount = 1;
//	descriptorWrites[1].pImageInfo = &imageInfo;
//
//	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//	descriptorWrites[2].dstSet = m_DescriptorSet;
//	descriptorWrites[2].dstBinding = 2;
//	descriptorWrites[2].dstArrayElement = 0;
//	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	descriptorWrites[2].descriptorCount = 1;
//	descriptorWrites[2].pImageInfo = &imageInfo2;
//	vkUpdateDescriptorSets(m_LogicDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
//}
//
//
//void VulkanApplication::CreateDescriptorSetLayout() {
//
//	VkDescriptorSetLayoutBinding vertexUniformBinding = {};
//	vertexUniformBinding.binding = 0;
//	vertexUniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//camera buffer
//	vertexUniformBinding.descriptorCount = 1;
//	vertexUniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	vertexUniformBinding.pImmutableSamplers = nullptr; // Optional
//	//cube m_ap
//	VkDescriptorSetLayoutBinding sampler1LayoutBinding = {};
//	sampler1LayoutBinding.binding = 1;
//	sampler1LayoutBinding.descriptorCount = 1;
//	sampler1LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	sampler1LayoutBinding.pImmutableSamplers = nullptr;
//	sampler1LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//	VkDescriptorSetLayoutBinding sampler2LayoutBinding = {};
//	sampler2LayoutBinding.binding = 2;
//	sampler2LayoutBinding.descriptorCount = 1;
//	sampler2LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//	sampler2LayoutBinding.pImmutableSamplers = nullptr;
//	sampler2LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//
//	std::array<VkDescriptorSetLayoutBinding, 3> bindings = { vertexUniformBinding, sampler1LayoutBinding,sampler2LayoutBinding };
//	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//	layoutInfo.pBindings = bindings.data();
//
//	if (vkCreateDescriptorSetLayout(m_LogicDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create descriptor set layout!");
//	}
//
//}

//vulkan functions
//void VulkanApplication::MainLoop() {
//	int i = 0;
//	while (!glfwWindowShouldClose(m_Window)) {
//		Do_movement();
//		ProcessInput(m_Window);
//		glfwPollEvents();
//		//DrawFrame();
//		++i;
//		i = i % 3;
//		
//	}
//
//	vkDeviceWaitIdle(m_LogicDevice);
//}

void VulkanApplication::Present(Image& image, uint32_t swapChainID, VkSemaphore* signalSemaphores) {
	
	VkImageCopy copy = {};
	copy.dstOffset = {};
	copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copy.dstSubresource.baseArrayLayer = 0;
	copy.dstSubresource.layerCount = 1;
	copy.dstSubresource.mipLevel = 0;
	copy.srcOffset = {};
	copy.srcSubresource = copy.dstSubresource;
	copy.extent = { m_SwapChainExtent.width,m_SwapChainExtent.height,0 };
	VkCommandBuffer cmd = CreateCommandBuffer(true);

	CopyImageToImage(cmd, m_SwapChainExtent.width, m_SwapChainExtent.height, image, m_SwapChainImages[swapChainID]);
	vkQueueWaitIdle(m_Queues.graphicsQueue);
	EndSingleTimeCommands(cmd);
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 2;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	//imageIndex = (imageIndex - 1) % 3;
	presentInfo.pImageIndices = &swapChainID;
	presentInfo.pResults = nullptr; // Optional
	VkResult result = vkQueuePresentKHR(m_Queues.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
		m_FramebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::exception("failed to present swap chain image!");
	}



}

void VulkanApplication::Cleanup() {


	//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	//	vkDestroySemaphore(m_LogicDevice, m_RenderFinishedSemaphores[i], nullptr);
	//	vkDestroySemaphore(m_LogicDevice, m_ImageAvailableSemaphores[i], nullptr);
	//	vkDestroyFence(m_LogicDevice, m_InFlightFences[i], nullptr);
	//}
	////vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	////vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
	////cleanupSwapChain();
	//vkDestroySampler(m_LogicDevice, m_TextureSampler, nullptr);
	//vkDestroySampler(m_LogicDevice, m_TextureSampler, nullptr);

	//vkDestroyDescriptorSetLayout(m_LogicDevice, m_DescriptorSetLayout, nullptr);
	//vkDestroyBuffer(m_LogicDevice, m_IndexBuffer, nullptr);
	//vkFreeMemory(m_LogicDevice, m_IndexBufferMemory, nullptr);

	//vkDestroyBuffer(m_LogicDevice, m_VertexBuffer, nullptr);
	//vkFreeMemory(m_LogicDevice, m_VertexBufferMemory, nullptr);

	//vkDestroyCommandPool(m_LogicDevice, m_CommandPool, nullptr);


	//vkDestroyDevice(m_LogicDevice, nullptr);
	////DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

	//vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	//vkDestroyInstance(m_Instance, nullptr);
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}



void VulkanApplication::CleanupSwapChain() {
	/*vkFreeCommandBuffers(m_LogicDevice, m_CommandPool, 1, &mCommandBuffer);


	vkDestroyFramebuffer(m_LogicDevice, m_FrameBufer, nullptr);


	vkDestroyPipeline(m_LogicDevice, m_Pipelines[m_RenderPasses["First"]][0], nullptr);
	vkDestroyPipelineLayout(m_LogicDevice, m_PipelineLayouts[m_RenderPasses["First"]][0], nullptr);*/
	/*vkDestroyRenderPass(m_LogicDevice, m_RenderPass, nullptr);
	for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
		vkDestroyImageView(m_LogicDevice, m_SwapChainImages[i].m_ImageView, nullptr);
	}
	vkDestroySwapchainKHR(m_LogicDevice, m_SwapChain, nullptr);


		vkDestroyBuffer(m_LogicDevice, m_CameraBuffer, nullptr);
		vkFreeMemory(m_LogicDevice, m_CameraBufferMemory, nullptr);

	vkDestroyDescriptorPool(m_LogicDevice, m_DescriptorPool, nullptr);*/


}


void VulkanApplication::DrawFrame(const VkCommandBuffer& cmd, Image& presentImage) {

	vkWaitForFences(m_LogicDevice, 1, &m_InFlightFences[currentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_LogicDevice, m_SwapChain, (std::numeric_limits<uint64_t>::max)(), m_ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::exception("failed to acquire swap chain image!");
	}

	if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
		throw std::exception("failed to record command buffer!");
	}

	vkQueueWaitIdle(m_Queues.graphicsQueue);
	UpdateUniformBuffer();
	//OpaqueUpdate();
	DifferPassUpdate();
	//SSRPassUpdate();
	ShadowUpdate();
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;
	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(m_LogicDevice, 1, &m_InFlightFences[currentFrame]);
	if (vkQueueSubmit(m_Queues.graphicsQueue, 1, &submitInfo, m_InFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::exception("failed to submit draw command buffer!");
	}

	VkSemaphore sems[2] = { m_ImageAvailableSemaphores[currentFrame]  ,m_RenderFinishedSemaphores[currentFrame] };
	Present(presentImage, imageIndex, sems);
	vkFreeCommandBuffers(m_LogicDevice, m_CommandPool, 1, &cmd);
	vkQueueWaitIdle(m_Queues.presentQueue);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}

void VulkanApplication::BeginRenderPass(const VkCommandBuffer& cmd, const VkRenderPass renderPass, const VkFramebuffer& framebuffer, const std::vector<VkClearValue>& clearColors,VkExtent2D area) {
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = area;

	renderPassInfo.clearValueCount = clearColors.size();
	renderPassInfo.pClearValues = clearColors.data();

	vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanApplication::BeginPipeline(const VkCommandBuffer& cmd, const Pipeline& pipeline, VkViewport viewPort, VkRect2D scissor) {
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.m_Pipeline);
	/*if (viewPort.width == 0)
	{
		viewPort.x = 0;
		viewPort.y = 0;
		viewPort.width = m_SwapChainExtent.width;
		viewPort.height = m_SwapChainExtent.height;
		viewPort.minDepth = 0.0f;
		viewPort.maxDepth = 1.0f;
	}
	if (scissor.extent.height == 0)
	{
		scissor.offset = { 0,0 };
		scissor.extent = m_SwapChainExtent;
	}
	vkCmdSetViewport(cmd, 0, 1, &viewPort);
	vkCmdSetScissor(cmd, 0, 1, &scissor);*/

	/*VkDeviceSize offsets[] = { 0 };

	for (auto& mesh : m_Pipeline_Models[pipeline])
	{
		vkCmdBindVertexBuffers(cmd, 0, 1, &mesh.m_Vertex.m_Buffer, offsets);
		vkCmdBindIndexBuffer(cmd, mesh.m_Index.m_Buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.m_Layout, 0, 1, m_Pipeline_DescriptorSets[pipeline].data(), 0, nullptr);
		vkCmdDrawIndexed(cmd, mesh.m_IndexSize, 1, 0, 0, 0);
	}*/
}

void VulkanApplication::DrawMesh(const VkCommandBuffer& cmd, const Model& model, const Pipeline& pipeline, const VkDescriptorSet* descriptorSet, size_t descriptorSetCount) {
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, &model.m_Vertex.m_Buffer, offsets);
	vkCmdBindIndexBuffer(cmd, model.m_Index.m_Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.m_Layout, 0, descriptorSetCount, descriptorSet, 0, nullptr);
	vkCmdDrawIndexed(cmd, model.m_IndexSize, 1, 0, 0, 0);
}


GLFWwindow* VulkanApplication::GetWindow()
{
	return m_Window;
}



void VulkanApplication::SkyboxPrepare() {
	Image skybox;
	VkSampler skyboxSampler = CreateTextureSampler();

	//skybox render pass
	VkRenderPass mRenderpass;
	//skybox pipeline
	Pipeline pipeline;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;


	//out color
	skybox = CreateCubeMap({ "..\\textures\\right.jpg","..\\textures\\left.jpg", "..\\textures\\top.jpg",
	"..\\textures\\bottom.jpg", "..\\textures\\front.jpg", "..\\textures\\back.jpg", });
	SetSampler(skybox, skyboxSampler);

	//create skybox render pass
	{
		auto outColorAttachment = CreateAttachmentDescriptionForColorAttachment(VK_FORMAT_B8G8R8A8_UNORM);
		auto outColorMapRes = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		std::vector <VkAttachmentReference> ref = { outColorMapRes };
		VkSubpassDescription destriputionOfsubpass = {};
		SetSubpassDescriptionColorAttachment(destriputionOfsubpass, ref.data(), ref.size());

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstSubpass = 0;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		mRenderpass = CreateRenderPass({ outColorAttachment }, { destriputionOfsubpass }, { subpassDependency });
	}

	//create descriptorSetLayout
	{
		auto layout1 = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto layout0 = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		descriptorSetLayout = CreateDescriptorSetLayout({ layout1 ,layout0 });
	}

	//create pipeline
	{
		//create shader
		auto fragShader = CreateShader("skybox.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		auto vertShader = CreateShader("skybox.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto bindingDescription = CreateVertexInputBindingDescription(0, VK_VERTEX_INPUT_RATE_VERTEX);
		//attribute
		auto attri0 = CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, position));
		auto attri1 = CreateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, normal));
		auto attri2 = CreateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, tangent));
		auto attri3 = CreateVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Aki, texCoord));
		auto attri4 = CreateVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, texCoord2));
		auto attri5 = CreateVertexInputAttributeDescription(5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Aki, texCoord3));



		//auto blendstate = CreatePipelineColorBlendAttachmentState();
		//std::vector<VkPipelineColorBlendAttachmentState> states = { blendstate};
		//auto blendstatecreateInfo = CreatePipelineColorBlendStateCreateInfo(states);
		VkPipelineDepthStencilStateCreateInfo dep = CreateDepthStencilStateCreateInfo();
		pipeline = CreatePipeline(mRenderpass, { fragShader ,vertShader }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { descriptorSetLayout }, dep,0);

	}

	//descriptor
	auto descriptorAllocate = CreateDescriptorSetAllocateInfo(&descriptorSetLayout);
	descriptorSet = CreateDescriptorSet({ descriptorAllocate });

	auto imagedescriptor = CreateWriteDescriptorSetForImage(skybox, 1, descriptorSet);
	auto bufferdescrptor = CreateWriteDescriptorSetForBuffer(m_CameraBuffer, 0, descriptorSet);


	UpdateDescriptorSets({ bufferdescrptor,imagedescriptor });

	//vertex
	Mesh* skyboxmesh = new Mesh();
	ModelReader::ReadModule("..\\models\\cube.obj", skyboxmesh);
	skyboxModel = UploadMesh(skyboxmesh);
	m_RenderPasses[0] = mRenderpass;
	skyboxPipeline = pipeline;
	//create frame buffer
	VkFramebuffer frameBuffer = CreateFrameBuffer({ m_RenderTarget }, mRenderpass,m_Width,m_Height);
	skyboxFramebuffer = frameBuffer;
	skyboxDescriptorSet = descriptorSet;
	skyboxDescriptorSetLayout = descriptorSetLayout;

}

void VulkanApplication::OpaquePrepare() {
	//opaque render pass
	VkRenderPass renderpass;
	//opaque pipeline
	Pipeline pipeline;

	VkDescriptorSetLayout descriptorSetLayout;

	//create opaque render pass
	{
		//read last color and save current
		auto outColorAttachment = CreateAttachmentDescriptionForColorAttachment(VK_FORMAT_B8G8R8A8_UNORM,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE);
		auto outColorMapRes = CreateAttachmentReference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		std::vector <VkAttachmentReference> ref = { outColorMapRes };

		auto depthAttachment = CreateAttachmentDescriptionForDepthAttachment(m_DepthBuffer.m_Format);
		auto depthRef = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);


		VkSubpassDescription subpassdescription = {};
		SetSubpassDescriptionBindPoint(subpassdescription);
		SetSubpassDescriptionColorAttachment(subpassdescription, ref.data(), ref.size());
		SetSubpassDescriptionDepthAttachment(subpassdescription, depthRef);

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstSubpass = 0;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		renderpass = CreateRenderPass({ depthAttachment, outColorAttachment }, { subpassdescription }, { subpassDependency });
	}

	//create descriptorSetLayout
	{
		//camera
		auto layout0 = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		//modeling
		auto layout1 = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		//texture
		auto layout2 = CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		descriptorSetLayout = CreateDescriptorSetLayout({ layout0, layout1 , layout2 });
	}

	//create pipeline
	{
		//create shader
		auto fragShader = CreateShader("opaque.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		auto vertShader = CreateShader("opaque.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto bindingDescription = CreateVertexInputBindingDescription(0, VK_VERTEX_INPUT_RATE_VERTEX);
		//attribute
		auto attri0 = CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, position));
		auto attri1 = CreateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, normal));
		auto attri2 = CreateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, tangent));
		auto attri3 = CreateVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Aki, texCoord));
		auto attri4 = CreateVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, texCoord2));
		auto attri5 = CreateVertexInputAttributeDescription(5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Aki, texCoord3));



		//auto blendstate = CreatePipelineColorBlendAttachmentState();
		//std::vector<VkPipelineColorBlendAttachmentState> states = { blendstate};
		//auto blendstatecreateInfo = CreatePipelineColorBlendStateCreateInfo(states);

		auto colorBlend = CreatePipelineColorBlendAttachmentState(VK_TRUE);
		auto colorBlendState = CreatePipelineColorBlendStateCreateInfo(&colorBlend, 1);
		VkPipelineDepthStencilStateCreateInfo dep = CreateDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE);
		pipeline = CreatePipeline(renderpass, { fragShader ,vertShader }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { descriptorSetLayout }, dep, 0,{ colorBlendState });

	}
	opaqueDescriptorSetLayout = descriptorSetLayout;

	//create frame buffer
	VkFramebuffer frameBuffer = CreateFrameBuffer({ m_DepthBuffer, m_RenderTarget }, renderpass,m_Width, m_Height);
	opaqueFramebuffer = frameBuffer;
	//m_Pipeline_DescriptorSets[pipeline].push_back(descriptorSet);
	m_RenderPasses[1] = renderpass;
	opaquePipeline = pipeline;
	Mesh* mesh = new Mesh();
	ModelReader::ReadModule("..\\models\\bunny.ply", mesh);
	mesh->m_Path = "..\\textures\\texture.jpg";
	UploadOpaqueMesh(mesh, 0);
}

void VulkanApplication::OpaqueUpdate() {
	Pipeline& pipeline = opaquePipeline;
	for (auto& model : opaqueModels)
	{
		glm::mat4x4 modeling = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4x4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4x4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
		CopyDataToBuffer(&modeling, model.m_Modeling);
	}
}

void VulkanApplication::DifferPassPrepare() {
	VkRenderPass differPass[2];
	normalImage = CreateImage(m_Width ,m_Height, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	colorImage = CreateImage(m_Width, m_Height,  COLOR_ATTACHMENT_FORMAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	positionImage = CreateImage(m_Width, m_Height, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	CreateImageView(normalImage,normalImage.m_Format);
	CreateImageView(colorImage, colorImage.m_Format);
	CreateImageView(positionImage, positionImage.m_Format);
	SetSampler(normalImage, m_DifferSampler);
	SetSampler(colorImage, m_DifferSampler);
	SetSampler(positionImage, m_DifferSampler);
	SetDescriptorImageInfo(normalImage);
	SetDescriptorImageInfo(colorImage);
	SetDescriptorImageInfo(positionImage);

	//Create Render pass
	{
		auto depthAtta = CreateAttachmentDescriptionForDepthAttachment(VK_FORMAT_D32_SFLOAT_S8_UINT,VK_ATTACHMENT_LOAD_OP_CLEAR,VK_ATTACHMENT_STORE_OP_STORE,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,VK_ATTACHMENT_LOAD_OP_CLEAR,VK_ATTACHMENT_STORE_OP_STORE);
		auto normalAtta = CreateAttachmentDescriptionForColorAttachment(DATA_ATTACHMENT_FORMAT,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		auto colorAtta = CreateAttachmentDescriptionForColorAttachment(COLOR_ATTACHMENT_FORMAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		auto positionAtta = CreateAttachmentDescriptionForColorAttachment(DATA_ATTACHMENT_FORMAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		auto depthRef = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		auto normalRef = CreateAttachmentReference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		auto colorRef = CreateAttachmentReference(2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		auto positionRef = CreateAttachmentReference(3, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		//subpass one
		std::vector<VkAttachmentReference> refs = { normalRef,colorRef,positionRef};
		VkSubpassDescription subpassdec1 = {};
		SetSubpassDescriptionBindPoint(subpassdec1);
		SetSubpassDescriptionColorAttachment(subpassdec1, refs.data(), refs.size());
		SetSubpassDescriptionDepthAttachment(subpassdec1, depthRef);
		/*auto dependency1 = CreateSubpassDependency(VK_SUBPASS_EXTERNAL, 0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);*/
		differPass[0] = CreateRenderPass({ depthAtta,normalAtta,colorAtta,positionAtta }, { subpassdec1 }, {  });
	}

	{
		auto depthAtta = CreateAttachmentDescriptionForDepthAttachment(DEPTH_ATTACHMENT_FORMAT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
			, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE);
		auto depthRef = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		auto renderTarget = CreateAttachmentDescriptionForColorAttachment(m_RenderTarget.m_Format, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR,VK_ATTACHMENT_STORE_OP_STORE);
		auto renderRef = CreateAttachmentReference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		//render pass two
		std::vector<VkAttachmentReference> refs2 = { renderRef };
		VkSubpassDescription subpassdec2 = {};
		SetSubpassDescriptionBindPoint(subpassdec2);
		SetSubpassDescriptionDepthAttachment(subpassdec2, depthRef);
		SetSubpassDescriptionColorAttachment(subpassdec2, refs2.data(), refs2.size());
	/*
		auto dependency2 = CreateSubpassDependency(VK_SUBPASS_EXTERNAL, 0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		*/
		differPass[1] = CreateRenderPass({ depthAtta, renderTarget }, { subpassdec2 }, {  });

	}



	//descriptor set layout
	{
		auto camera = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		auto modelingMatrix = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		auto textureImage = CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		differ_descriptorSetLayouts[0] = CreateDescriptorSetLayout({ camera,modelingMatrix, textureImage });

		auto normalImage = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto colorImage = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto positionImage = CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto depthBuffer = CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

		auto lightInfo = CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto cameraInfo = CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto shadowDepthBuffer = CreateDescriptorSetLayoutBinding(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto shadowMatrixBuffer = CreateDescriptorSetLayoutBinding(7, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		differ_descriptorSetLayouts[1] = CreateDescriptorSetLayout({ normalImage,colorImage, positionImage,lightInfo,depthBuffer,cameraInfo,shadowDepthBuffer,shadowMatrixBuffer });
	}


	//pipeline
	{
		auto vertshader1 = CreateShader("differ.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragshader1 = CreateShader("differ.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

		auto vertshader2 = CreateShader("offscreen.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragshader2 = CreateShader("offscreen.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

		auto bindingDescription = CreateVertexInputBindingDescription(0, VK_VERTEX_INPUT_RATE_VERTEX);
		//attribute
		auto attri0 = CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, position));
		auto attri1 = CreateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, normal));
		auto attri2 = CreateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, tangent));
		auto attri3 = CreateVertexInputAttributeDescription(3,0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Aki, texCoord));
		auto attri4 = CreateVertexInputAttributeDescription(4, 0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex_Aki, texCoord2));
		auto attri5 = CreateVertexInputAttributeDescription(5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Aki, texCoord3));

		auto colorBlend = CreatePipelineColorBlendAttachmentState(false);
		VkPipelineColorBlendAttachmentState states[4] = { colorBlend ,colorBlend ,colorBlend};
		auto colorBlendCreateInfo = CreatePipelineColorBlendStateCreateInfo(states, 3);
		

		VkPipelineDepthStencilStateCreateInfo depOpen = CreateDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE,VK_COMPARE_OP_LESS,0.0f,1.0f,VK_FALSE,VK_TRUE);
		VkStencilOpState face = {};
		face.compareMask = 0xff;
		face.compareOp = VK_COMPARE_OP_ALWAYS;
		face.passOp = VK_STENCIL_OP_REPLACE;
		face.failOp = VK_STENCIL_OP_REPLACE;
		face.depthFailOp = VK_STENCIL_OP_KEEP;
		face.writeMask = 0xff;
		face.reference = 1.0f;

		VkStencilOpState back = face;
		depOpen.front = face;
		depOpen.back = back;
		differPipelines[0] = CreatePipeline(differPass[0], { vertshader1,fragshader1 }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { differ_descriptorSetLayouts[0] }, depOpen,0, { colorBlendCreateInfo });
		auto colorBlendclose = CreatePipelineColorBlendAttachmentState(false,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD);
		VkPipelineColorBlendAttachmentState states2[1] = { colorBlendclose};
		auto colorBlendCreateInfo2 = CreatePipelineColorBlendStateCreateInfo(states2, 1);
		auto dep = CreateDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, 0, 1, VK_FALSE, VK_TRUE);
		VkStencilOpState front = {};
		front.compareMask = 0xff;
		front.writeMask = 0xff;
		front.compareOp = VK_COMPARE_OP_EQUAL;
		front.depthFailOp = VK_STENCIL_OP_KEEP;
		front.failOp = VK_STENCIL_OP_KEEP;
		front.passOp = VK_STENCIL_OP_KEEP;
		front.reference = 1;
		dep.front = front;
		dep.back = front;
		differPipelines[1] = CreatePipeline(differPass[1], { vertshader2,fragshader2 }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { differ_descriptorSetLayouts[1] }, dep,0, { colorBlendCreateInfo2 });
	}

	//framebuffer
	differ_framebuffer[0] = CreateFrameBuffer({ m_DepthBuffer,normalImage,colorImage,positionImage }, differPass[0], m_Width, m_Height);
	differ_framebuffer[1] = CreateFrameBuffer({ m_DepthBuffer,m_RenderTarget}, differPass[1], m_Width, m_Height);



	m_RenderPasses[2] = differPass[0];
	m_RenderPasses[3] = differPass[1];

	//upload mesh
	Mesh* mesh = new Mesh();
	ModelReader::ReadModule("..\\models\\bunny.ply", mesh);
	mesh->m_Path = "..\\textures\\texture.jpg";
	UploadDifferMesh(mesh, 0);
	glm::mat4x4 modeling = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4x4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4x4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));

	CopyDataToBuffer(&modeling, differModels[differModels.size() - 1].m_Modeling);

	Mesh* plane = new Mesh();
	ModelReader::ReadModule("..\\models\\plane.obj", plane);
	plane->m_Path = "..\\textures\\white.png";
	UploadDifferMesh(plane, 0);

	modeling = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4x4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4x4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
	CopyDataToBuffer(&modeling,differModels[differModels.size()-1].m_Modeling);



	//create second descriptor set
	auto desAllo = CreateDescriptorSetAllocateInfo(&differ_descriptorSetLayouts[1]);
	differ_secondStageDescriptorSet = CreateDescriptorSet({ desAllo });
	auto normalWrite = CreateWriteDescriptorSetForImage(normalImage,0, differ_secondStageDescriptorSet);
	auto colorWrite = CreateWriteDescriptorSetForImage(colorImage, 1, differ_secondStageDescriptorSet);
	auto posWrite = CreateWriteDescriptorSetForImage(positionImage, 2, differ_secondStageDescriptorSet);
	auto depthWrite = CreateWriteDescriptorSetForImage(m_DepthBuffer, 3, differ_secondStageDescriptorSet);
	auto lightInfoWrite = CreateWriteDescriptorSetForBuffer(light, 4, differ_secondStageDescriptorSet);
	auto cameraWrite = CreateWriteDescriptorSetForBuffer(m_CameraBuffer, 5, differ_secondStageDescriptorSet);
	auto shadowdepthWrite = CreateWriteDescriptorSetForImage(shadowDepth, 6, differ_secondStageDescriptorSet);
	auto shadowMatWrite = CreateWriteDescriptorSetForBuffer(lightCameraBuffer, 7, differ_secondStageDescriptorSet);
	UpdateDescriptorSets({ normalWrite ,colorWrite ,posWrite ,depthWrite,lightInfoWrite,cameraWrite,shadowdepthWrite,shadowMatWrite });

}


void VulkanApplication::UploadDifferMesh(Mesh* mesh, int submesh) {
	mesh->mSubmeshes[submesh]->mVertexBuffer = CreateVertexBuffer(mesh->mSubmeshes[submesh]->mVertices);
	mesh->mSubmeshes[submesh]->mIndexBuffer = CreateIndexBuffer(mesh->mSubmeshes[submesh]->mIndicies);

	Model mod = { mesh->mVertexBuffer,mesh->mIndexBuffer };
	mod.m_IndexSize = mesh->mSubmeshes[submesh]->mIndicies.size();

	//create descriptor set
	VkDescriptorSet differset;

	if (mesh->m_Path != nullptr)
	{
		mod.m_Texture = CreateTextureImage(mesh->m_Path);
	}
	else
	{
		mod.m_Texture = m_WhiteImage;
	}

	//create descriptor set
	mod.m_Modeling = CreateBuffer(sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	MapBuffer(mod.m_Modeling);
	auto allocateinfo = CreateDescriptorSetAllocateInfo(&differ_descriptorSetLayouts[0]);
	differset = CreateDescriptorSet({ allocateinfo });
	differDescriptorSets.push_back(differset);
	auto cameraBuffer = CreateWriteDescriptorSetForBuffer(m_CameraBuffer, 0, differset);
	auto modelBuffer = CreateWriteDescriptorSetForBuffer(mod.m_Modeling, 1, differset);
	auto imageWrite = CreateWriteDescriptorSetForImage(mod.m_Texture, 2, differset);
	UpdateDescriptorSets({ imageWrite, cameraBuffer ,modelBuffer });
	differModels.push_back(mod);
}

void VulkanApplication::DifferPassUpdate() {
	/*for (auto& model : differModels)
	{
		CopyDataToBuffer(&model.m_ModlingMat, model.m_Modeling);
	}*/
}



void VulkanApplication::SSRPassPrepare() {
	VkRenderPass SSRRenderPass;
	SSR_RenderTarget = CreateImage(m_Width, m_Height, COLOR_ATTACHMENT_FORMAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	CreateImageView(SSR_RenderTarget, SSR_RenderTarget.m_Format);
	SetSampler(SSR_RenderTarget, m_TextureSampler);
	SetDescriptorImageInfo(SSR_RenderTarget);
	//Create Render pass
	{
		auto depthAtta = CreateAttachmentDescriptionForDepthAttachment(DEPTH_ATTACHMENT_FORMAT,VK_ATTACHMENT_LOAD_OP_LOAD,VK_ATTACHMENT_STORE_OP_DONT_CARE,VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		,VK_ATTACHMENT_LOAD_OP_LOAD,VK_ATTACHMENT_STORE_OP_DONT_CARE);
		auto renderTarget = CreateAttachmentDescriptionForColorAttachment(COLOR_ATTACHMENT_FORMAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR);
		auto renderRef = CreateAttachmentReference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		auto depthRef = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		//subpass
		std::vector<VkAttachmentReference> refs = {renderRef};
		VkSubpassDescription subpassdec1 = {};
		SetSubpassDescriptionBindPoint(subpassdec1);
		SetSubpassDescriptionColorAttachment(subpassdec1, refs.data(), refs.size());
		SetSubpassDescriptionDepthAttachment(subpassdec1, depthRef);
		auto dependency1 = CreateSubpassDependency(VK_SUBPASS_EXTERNAL, 0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		SSRRenderPass = CreateRenderPass({ depthAtta,renderTarget }, { subpassdec1 }, { dependency1 });
	}

	//descriptor set layout
	{

		auto camera = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		auto normalTex = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto colorTex = CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto worldTex = CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto depthTex = CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto modeling = CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

		SSR_descriptorSetLayout = CreateDescriptorSetLayout({ camera,normalTex, colorTex,worldTex,depthTex,modeling });
	}


	//pipeline
	{
		auto vertshader = CreateShader("SSR.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragshader = CreateShader("SSR.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

		auto bindingDescription = CreateVertexInputBindingDescription(0, VK_VERTEX_INPUT_RATE_VERTEX);
	
		auto attri0 = CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, position));
		auto attri1 = CreateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, normal));
		auto attri2 = CreateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, tangent));
		auto attri3 = CreateVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Aki, texCoord));
		auto attri4 = CreateVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, texCoord2));
		auto attri5 = CreateVertexInputAttributeDescription(5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Aki, texCoord3));


		auto colorBlend = CreatePipelineColorBlendAttachmentState(false);

		VkPipelineColorBlendAttachmentState states[1] = { colorBlend };
		auto colorBlendCreateInfo = CreatePipelineColorBlendStateCreateInfo(states, 1);
		//TODO: stencil compare
		auto dep = CreateDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS, 0, 1, VK_FALSE, VK_TRUE);
		VkStencilOpState front = {};
		front.compareMask = 0xff;
		front.writeMask = 0xff;
		front.compareOp = VK_COMPARE_OP_EQUAL;
		front.depthFailOp = VK_STENCIL_OP_KEEP;
		front.failOp = VK_STENCIL_OP_KEEP;
		front.passOp = VK_STENCIL_OP_KEEP;
		front.reference = 1;
		dep.front = front;
		dep.back = front;
		SSRPipelines = CreatePipeline(SSRRenderPass, { vertshader,fragshader }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { SSR_descriptorSetLayout}, dep, 0, { colorBlendCreateInfo });
	}

	//framebuffer
	SSR_framebuffer = CreateFrameBuffer({ m_DepthBuffer,SSR_RenderTarget }, SSRRenderPass, m_Width, m_Height);
	m_RenderPasses[4] = SSRRenderPass;

	Mesh* plane = new Mesh();
	ModelReader::ReadModule("..\\models\\plane.obj", plane);
	plane->m_Path = "..\\textures\\white.png";
	UploadSSRMesh(plane, 0);

}
void VulkanApplication::SSRPassUpdate() {
	for (auto& model : SSRModels)
	{
		glm::mat4x4 modeling = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4x4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4x4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
		CopyDataToBuffer(&modeling, model.m_Modeling);
	}

}
void VulkanApplication::UploadSSRMesh(Mesh* mesh, int submesh) {
	mesh->mSubmeshes[submesh]->mVertexBuffer = CreateVertexBuffer(mesh->mSubmeshes[submesh]->mVertices);
	mesh->mSubmeshes[submesh]->mIndexBuffer = CreateIndexBuffer(mesh->mSubmeshes[submesh]->mIndicies);

	Model mod = { mesh->mVertexBuffer,mesh->mIndexBuffer };
	mod.m_IndexSize = mesh->mSubmeshes[submesh]->mIndicies.size();

	//create descriptor set
	VkDescriptorSet ssrset;

	if (mesh->m_Path != nullptr)
	{
		mod.m_Texture = CreateTextureImage(mesh->m_Path);
	}
	else
	{
		mod.m_Texture = m_WhiteImage;
	}

	//create descriptor set
	mod.m_Modeling = CreateBuffer(sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	MapBuffer(mod.m_Modeling);
	auto allocateinfo = CreateDescriptorSetAllocateInfo(&SSR_descriptorSetLayout);
	ssrset = CreateDescriptorSet({ allocateinfo });
	SSRDescriptorSets.push_back(ssrset);
	auto cameraBuffer = CreateWriteDescriptorSetForBuffer(m_CameraBuffer, 0, ssrset);
	auto normalWrite = CreateWriteDescriptorSetForImage(normalImage, 1, ssrset);
	auto colorWrite = CreateWriteDescriptorSetForImage(colorImage, 2, ssrset);
	auto posWrite = CreateWriteDescriptorSetForImage(positionImage, 3, ssrset);
	auto depthWrite = CreateWriteDescriptorSetForImage(m_DepthBuffer, 4, ssrset);
	auto modelingWrite = CreateWriteDescriptorSetForBuffer(mod.m_Modeling, 5, ssrset);
	UpdateDescriptorSets({ normalWrite, cameraBuffer ,colorWrite,posWrite,depthWrite,modelingWrite });
	SSRModels.push_back(mod);
	

}


std::array<glm::vec3, 4> VulkanApplication::CaluculateCameraCorners(const Camera& camera, float step) {
	std::array<glm::vec3, 4> result;
	//degree to radians
	float fovRadians = glm::radians(fov);
	//make sure the step is between 0% to 100%(in near and far plane)
	step = std::clamp(step, 0.0f, 1.0f);
	float z = - Lerp(camera.nearClip, camera.farClip, step);
	float h = z * std::tanf(fovRadians * 0.5f);
	float w = h * camera.aspect;
	result[0] = glm::vec3(w, h, z);
	result[1] = glm::vec3(w, -h, z);
	result[2] = glm::vec3(-w, -h, z);
	result[3] = glm::vec3(-w, h, z);
	return result;
};

glm::mat4x4 VulkanApplication::CalcuLightCameraFrustum(const CameraCorner& cameraCornersInCameraCoord, const glm::mat4x4& viewMat, LightInfo light) {
	std::array<glm::vec3, 4> nearCorners;
	std::array<glm::vec3, 4> farCorners;

	if (light.type == LightType::DIRECT_LIGHT)
	{
		glm::mat4x4 viewMatInv = glm::inverse(viewMat);

		glm::mat4x4 worldToLight = glm::lookAt(light.lightPos, glm::vec3(0.0f, 0.0f, 0.0f), up);
		glm::mat4x4 lightToWorld = glm::inverse(worldToLight);

		//translate corners to light space
		for (int i = 0; i < 4; ++i)
		{
			nearCorners[i] =  viewMatInv * glm::vec4(cameraCornersInCameraCoord.nearCorners[i], 1.0);
			nearCorners[i] = worldToLight * glm::vec4(nearCorners[i],1.0);
			farCorners[i] = worldToLight * viewMatInv * glm::vec4(cameraCornersInCameraCoord.farCorners[i], 1.0);
		}


		float minX = (std::numeric_limits<float>::max)();
		float maxX = -(std::numeric_limits<float>::max)();
		float minY = (std::numeric_limits<float>::max)();
		float maxY = -(std::numeric_limits<float>::max)();
		float minZ = (std::numeric_limits<float>::max)();
		float maxZ = -(std::numeric_limits<float>::max)();

		for (int i = 0; i < 4; ++i)
		{
			//min x
			if (minX > nearCorners[i].x)
			{
				minX = nearCorners[i].x;
			}
			if (minX > farCorners[i].x)
			{
				minX = farCorners[i].x;
			}

			//min y
			if (minY > nearCorners[i].y)
			{
				minY = nearCorners[i].y;
			}
			if (minY > farCorners[i].y)
			{
				minY = farCorners[i].y;
			}

			//min z
			if (minZ > nearCorners[i].z)
			{
				minZ = nearCorners[i].z;
			}
			if (minZ > farCorners[i].z)
			{
				minZ = farCorners[i].z;
			}

			//max x
			if (maxX < nearCorners[i].x)
			{
				maxX = nearCorners[i].x;
			}
			if (maxX < farCorners[i].x)
			{
				maxX = farCorners[i].x;
			}

			//max y
			if (maxY < nearCorners[i].y)
			{
				maxY = nearCorners[i].y;
			}
			if (maxY < farCorners[i].y)
			{
				maxY = farCorners[i].y;
			}

			//max z
			if (maxZ < nearCorners[i].z)
			{
				maxZ = nearCorners[i].z;
			}
			if (maxZ < farCorners[i].z)
			{
				maxZ = farCorners[i].z;
			}
		}

		//near far plane in light coord because of z is always less that 0 in light coord, 
		//near plane should be greater z
		nearCorners[0] = glm::vec3(minX, minY, maxZ);
		nearCorners[1] = glm::vec3(maxX, minY, maxZ);
		nearCorners[2] = glm::vec3(maxX, maxY, maxZ);
		nearCorners[3] = glm::vec3(minX, maxY, maxZ);


		farCorners[0] = glm::vec3(minX, minY, minZ);
		farCorners[1] = glm::vec3(maxX, minY, minZ);
		farCorners[2] = glm::vec3(maxX, maxY, minZ);
		farCorners[3] = glm::vec3(minX, maxY, minZ);
		//{x = 18.9480820f, y = 32.1282730f, z = -15.7848473f}

		glm::vec3 pos = nearCorners[0] + (nearCorners[2] - nearCorners[0]) * 0.5f;
		glm::vec3 lightPos = lightToWorld * glm::vec4(pos, 1.0f);
		//dirLightCamera.transform.rotation = dirLight.transform.rotation;
		float nearClipPlane = 0;
		float farClipPlane = maxZ - minZ;
		float h = glm::length(nearCorners[1] - nearCorners[2]) * 0.5f;
		float left = glm::length(nearCorners[0] - nearCorners[1]) * 0.5f;
		auto view = glm::lookAt(lightPos, lightPos + light.lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
		auto proj = glm::ortho(-left, left, -h, h, nearClipPlane, farClipPlane);
		//proj = glm::perspective(glm::radians(60.0f),1.0f,0.1f,1000.0f);
		proj[1][1] *= -1.0f;
		return proj * view;
	}
}

void VulkanApplication::CalculateFourLightCameras(const CameraCorner& cameraCornersInCameraCoord, const CameraViewInfo& camera, LightInfo light, glm::mat4* results) {
	CameraCorner cascadesCorners[4] = {};
	for (int i = 0; i < 4; i++)
	{
		cascadesCorners[0].nearCorners[i] = Lerp(cameraCornersInCameraCoord.nearCorners[i], cameraCornersInCameraCoord.farCorners[i], LIGHT_CASCADES_STEP[0]);
		cascadesCorners[0].farCorners[i] = Lerp(cameraCornersInCameraCoord.nearCorners[i], cameraCornersInCameraCoord.farCorners[i], LIGHT_CASCADES_STEP[1]);

		cascadesCorners[1].nearCorners[i] = cascadesCorners[0].nearCorners[i];
		cascadesCorners[1].farCorners[i] = Lerp(cameraCornersInCameraCoord.nearCorners[i], cameraCornersInCameraCoord.farCorners[i], LIGHT_CASCADES_STEP[2]);

		cascadesCorners[2].nearCorners[i] = cascadesCorners[0].nearCorners[i];
		cascadesCorners[2].farCorners[i] = Lerp(cameraCornersInCameraCoord.nearCorners[i], cameraCornersInCameraCoord.farCorners[i ], LIGHT_CASCADES_STEP[3]);

		cascadesCorners[3].nearCorners[i] = cascadesCorners[0].nearCorners[i];
		cascadesCorners[3].farCorners[i] = Lerp(cameraCornersInCameraCoord.nearCorners[i], cameraCornersInCameraCoord.farCorners[i], LIGHT_CASCADES_STEP[4]);
	}

	for (int i = 0; i < 4; ++i)
	{
		results[i] = CalcuLightCameraFrustum(cascadesCorners[i], camera.view, light);
	}

	return;
}

std::array<glm::mat4x4, 4> VulkanApplication::CalculateCameraCascadesFrustms(const Camera& camera, const LightInfo& light) {
	if (light.type != DIRECT_LIGHT)
	{
		return std::array<glm::mat4x4, 4>();
	}

	CameraCorner corner;
	corner.nearCorners = CaluculateCameraCorners(camera, 0);
	corner.farCorners = CaluculateCameraCorners(camera, 1);

	std::array<glm::mat4x4, 4> result;
	CalculateFourLightCameras(corner, camera.viewInfo, light, result.data());
	return result;

}

void VulkanApplication::ShadowPrepare() {
	
	//render pass
	{
		auto depthBuffer = CreateAttachmentDescriptionForDepthAttachment(DEPTH_ATTACHMENT_FORMAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE);
		auto depthbufferRef = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		VkSubpassDescription subpassdesc = {};
		SetSubpassDescriptionBindPoint(subpassdesc);
		SetSubpassDescriptionDepthAttachment(subpassdesc, depthbufferRef);

		shadowPass = CreateRenderPass({ depthBuffer }, { subpassdesc }, {  });
	}
	//descriptor set layout
	{
		auto lightCameraDescriptor = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_GEOMETRY_BIT, 1);
		//auto lightCameraDescriptor = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

		auto modelingDescriptor = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		shadowLayout = CreateDescriptorSetLayout({ lightCameraDescriptor,modelingDescriptor });
	}


	//pipeline
	{
		auto vertShader = CreateShader("..//shader//shadow.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragShader = CreateShader("..//shader//shadow.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		auto gemoShader = CreateShader("..//shader//shadow.geom", VK_SHADER_STAGE_GEOMETRY_BIT);
		auto bindingDescription = CreateVertexInputBindingDescription(0, VK_VERTEX_INPUT_RATE_VERTEX);
		//attribute
		auto attri0 = CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, position));
		auto attri1 = CreateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, normal));
		auto attri2 = CreateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, tangent));
		auto attri3 = CreateVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Aki, texCoord));
		auto attri4 = CreateVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, texCoord2));
		auto attri5 = CreateVertexInputAttributeDescription(5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Aki, texCoord3));

		VkPipelineDepthStencilStateCreateInfo depthTestOn = CreateDepthStencilStateCreateInfo(VK_TRUE,VK_TRUE,VK_COMPARE_OP_LESS,0.0F,1.0F,VK_FALSE,VK_FALSE);		
		shadowPipeline = CreatePipeline(shadowPass, { vertShader,fragShader,gemoShader }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { shadowLayout }, depthTestOn, 0);
		//shadowPipeline = CreatePipeline(shadowPass, { vertShader,fragShader }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { shadowLayout }, depthTestOn, 0);

	}

	Mesh* mesh = new Mesh();
	ModelReader::ReadModule("..\\models\\bunny.ply", mesh);
	mesh->m_Path = "..\\textures\\texture.jpg";
	Model bunnyModel = UploadMesh(mesh, 0);
	glm::mat4x4 modeling = glm::translate(glm::mat4x4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4x4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4x4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));

	//create descriptor set
	bunnyModel.m_Modeling = CreateBuffer(sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	MapBuffer(bunnyModel.m_Modeling);
	CopyDataToBuffer(&modeling, bunnyModel.m_Modeling);
	auto allocateinfo = CreateDescriptorSetAllocateInfo(&shadowLayout);
	VkDescriptorSet shadowSet = CreateDescriptorSet({ allocateinfo });
	SSRDescriptorSets.push_back(shadowSet);
	auto cameraBuffer = CreateWriteDescriptorSetForBuffer(lightCameraBuffer, 1, shadowSet);
	auto modelingWrite = CreateWriteDescriptorSetForBuffer(bunnyModel.m_Modeling, 0, shadowSet);
	UpdateDescriptorSets({cameraBuffer ,modelingWrite });
	shadowSets.push_back(shadowSet);
	shadowModels.push_back(bunnyModel);

	//create frame buffer
	shadow_Frambuffer = CreateFrameBuffer({ shadowDepth },shadowPass,4096, 4096);


}
void VulkanApplication::ShadowUpdate() {
	std::array<glm::mat4x4, 4> lightCamera;
	lightCamera =CalculateCameraCascadesFrustms(camera, sunLight);
	//lightCamera[0] = camera.viewInfo.proj * camera.viewInfo.view;
	CopyDataToBuffer(lightCamera.data(), lightCameraBuffer);
}

void VulkanApplication::WaterPrepare(){
	std::string texture = "Water-0341";
	VkRenderPass waterPass;
	Pipeline waterPipeline;
	VkFramebuffer waterFramebuffer;
	VkDescriptorSetLayout waterDescriptorSetLayout;
	VkDescriptorSet waterDescriptorSet;
	Model waterModel;
	Image waterTexture = CreateTextureImage("Water-0341.jpg");
	Image waterNormalTexture = CreateTextureImage("Water-0341normal.jpg");
	WaterParameter waterPara = {};
	Buffer waterParaBuffer = CreateBuffer(sizeof(WaterParameter), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//Render Pass
	{
		auto colorAttach = CreateAttachmentDescriptionForColorAttachment(COLOR_ATTACHMENT_FORMAT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,VK_ATTACHMENT_LOAD_OP_LOAD);
		auto colorRef = CreateAttachmentReference(1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		auto depthAttach = CreateAttachmentDescriptionForDepthAttachment(DEPTH_ATTACHMENT_FORMAT, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_STORE_OP_STORE);
		auto depthRef = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
		VkSubpassDescription subpassDes;
		SetSubpassDescriptionBindPoint(subpassDes);
		SetSubpassDescriptionColorAttachment(subpassDes, &colorRef, 1);
		SetSubpassDescriptionDepthAttachment(subpassDes, depthRef);
		waterPass = CreateRenderPass({ colorAttach,depthAttach }, { subpassDes }, {});
	}

	//descriptor
	{
		auto waterTexture = CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto waterNormalTexture = CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto depthTexture = CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto waterParameters = CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
		auto modelingMatrix = CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		auto lightInfo = CreateDescriptorSetLayoutBinding(5, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

		waterDescriptorSetLayout = CreateDescriptorSetLayout({ waterTexture ,waterNormalTexture , waterParameters, depthTexture ,modelingMatrix,lightInfo });
	}

	//pipeline
	{
		auto vertShader = CreateShader("..//shader//water.vert", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragShader = CreateShader("..//shader//water.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
		//auto gemoShader = CreateShader("..//shader//shadow.geom", VK_SHADER_STAGE_GEOMETRY_BIT);
		auto bindingDescription = CreateVertexInputBindingDescription(0, VK_VERTEX_INPUT_RATE_VERTEX);
		//attribute
		auto attri0 = CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, position));
		auto attri1 = CreateVertexInputAttributeDescription(1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, normal));
		auto attri2 = CreateVertexInputAttributeDescription(2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, tangent));
		auto attri3 = CreateVertexInputAttributeDescription(3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Aki, texCoord));
		auto attri4 = CreateVertexInputAttributeDescription(4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Aki, texCoord2));
		auto attri5 = CreateVertexInputAttributeDescription(5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Aki, texCoord3));

		VkPipelineDepthStencilStateCreateInfo depthTestOnButNotWrite = CreateDepthStencilStateCreateInfo(VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS, 0.0F, 1.0F, VK_FALSE, VK_FALSE);
		auto colorBlend = CreatePipelineColorBlendAttachmentState(VK_TRUE);
		auto colorblendCreateInfo = CreatePipelineColorBlendStateCreateInfo(&colorBlend, 1);
		
		waterPipeline = CreatePipeline(waterPass, { vertShader,fragShader }, { bindingDescription }, { attri0 ,attri1 ,attri2 ,attri3 ,attri4,attri5 }, { waterDescriptorSetLayout }, depthTestOnButNotWrite, 0, { colorblendCreateInfo });
	}

	//Frame buffer
	{
		waterFramebuffer = CreateFrameBuffer({ m_DepthBuffer,m_RenderTarget }, waterPass, m_Width, m_Height);
	
	}


	Mesh* plane = new Mesh();
	ModelReader::ReadModule("..\\models\\plane.obj", plane);
	//plane->m_Path = "..\\textures\\white.png";
	waterModel = UploadMesh(plane);

	//descriptor set
	auto setallo = CreateDescriptorSetAllocateInfo(&waterDescriptorSetLayout, 1);
	waterDescriptorSet = CreateDescriptorSet({ setallo });
	auto waterTexWrite = CreateWriteDescriptorSetForImage(waterTexture, 0, waterDescriptorSet);
	auto waterNormalTextureWrite = CreateWriteDescriptorSetForImage(waterNormalTexture, 1, waterDescriptorSet);
	auto depthTextureWrite = CreateWriteDescriptorSetForImage(m_DepthBuffer, 2, waterDescriptorSet);
	auto waterParameterWrite = CreateWriteDescriptorSetForBuffer(waterParaBuffer, 3, waterDescriptorSet);
	auto modelingWrite = CreateWriteDescriptorSetForBuffer(waterModel.m_Modeling, 4, waterDescriptorSet);
	auto lightInfoWrite = CreateWriteDescriptorSetForBuffer(light, 5, waterDescriptorSet);


}


void VulkanApplication::LoadWaterMesh(Mesh* mesh, uint32_t submesh) {
	


}

void WaterUpedate() {}