#include"VulkanApplication.h"
#include <assert.h>
#include<iostream>
#include <stdexcept>
#include<set>
#include<chrono>
#include <gtc/matrix_transform.hpp>
#include <stdarg.h>
#include <fstream>
#include<array>
#include"Mesh.h"
#include<stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include"ModelReader.h"
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
struct LightInfo {
	
	glm::vec3 lightPos;	
	float  ambientStrenth;
	glm::vec3 lightColor;
	
	
};


void VulkanApplication::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		mCamera += mCameraForward * 0.2f;
	}


	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		mCamera -= mCameraForward * 0.2f;
	}
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
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
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
	: mValidationLayers({
		"VK_LAYER_KHRONOS_validation"
		})
	, mRequiredExtentions()
	, mDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME })
#ifdef NDEBUG
	, mEnableValidationLayer(false)
#else
	, mEnableValidationLayer(true)
#endif
	, mDebugCallback(nullptr)
	, mWindow(nullptr)
{

};

VulkanApplication::VulkanApplication() :Application("", 0, 0)
, mValidationLayers({
	"VK_LAYER_KHRONOS_validation"
	})
	, mRequiredExtentions()
	, mDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME })
#ifdef NDEBUG
	, mEnableValidationLayer(false)
#else
	, mEnableValidationLayer(true)
#endif
	, mDebugCallback(nullptr)
	, mWindow(nullptr) {

}

void VulkanApplication::Init() {
	if (mQueueFlags == 0)
	{
		mQueueFlags = QUEUE_COMPUTE_BIT | QUEUE_GRAPHICS_BIT | QUEUE_TRANSFER_BIT;
	}

	if (mWindow == nullptr)
	{
		CreateMyWindow();
	}
	CreateInstance();
	CreateDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateDevice();
	CreateSwapChain();
	CreateImageViews();

	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateCommandPool();
	CreateTextureImage("texture.jpg");
	CreateTextureImageView();
	CreateTextureSampler();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
}

void VulkanApplication::Run() {
	CreateSyncObjects();
	MainLoop();
}


void VulkanApplication::SetResizable(bool state) {


}

bool VulkanApplication::Familys::isComplete(unsigned int mQueueFlags) {

	bool pre = presentFamily.has_value();
	bool com = !(mQueueFlags & QUEUE_COMPUTE_BIT) || computeFamily.has_value();
	bool gra = !(mQueueFlags & QUEUE_GRAPHICS_BIT) || graphicsFamily.has_value();
	bool tra = !(mQueueFlags & QUEUE_TRANSFER_BIT) || transferFamily.has_value();
	bool spar = !(mQueueFlags & QUEUE_SPARSE_BINDING_BIT) || sparseBindingFamily.has_value();
	return pre && com&& gra&& tra&& spar;
}

//help functions

void VulkanApplication::SetApplicationInfo(std::string applicationName, int width, int height) {
	mApplicationName = applicationName;
	mWidth = width;
	mHeight = height;
}


void VulkanApplication::SetRequiredValadationLayers(std::vector<const char*> valadationLayers) {
	mValidationLayers = valadationLayers;
}
void VulkanApplication::SetRequiredExtentions(std::vector<const char*> extentions) {
	mRequiredExtentions = extentions;
}

void VulkanApplication::SetWindowHandle(HWND window, HINSTANCE handle) {
	mHWND = window;
	mHINSTANCE = handle;
}

void VulkanApplication::SetDeviceExtensions(std::vector<const char*> extentions) {
	mDeviceExtensions = extentions;
}
void VulkanApplication::SetQueueFlag(QueueFlagBits flag) {
	mQueueFlags = flag;
}

void VulkanApplication::CreateMyWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	mWindow = glfwCreateWindow(mWidth, mHeight, mApplicationName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
}


void VulkanApplication::CreateInstance() {

	if (mEnableValidationLayer && !CheckValidationLayerSupport())
	{
		throw std::exception("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = mApplicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VoidEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (mRequiredExtentions.size() == 0)
	{
		mRequiredExtentions = GetRequiredExtensions();
	}
	createInfo.enabledExtensionCount = static_cast<uint32_t>(mRequiredExtentions.size());
	createInfo.ppEnabledExtensionNames = mRequiredExtentions.data();
	createInfo.enabledLayerCount = 0;
	if (mEnableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
		throw std::exception("failed to create instance!");
	}
}

bool VulkanApplication::CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : mValidationLayers) {
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

	if (mEnableValidationLayer) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void VulkanApplication::CreateDebugCallback() {

	if (!mEnableValidationLayer) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	if (mDebugCallback != nullptr)
	{
		createInfo.pfnUserCallback = mDebugCallback;

	}
	else
	{
		createInfo.pfnUserCallback = debugCallback;
	}
	createInfo.pUserData = nullptr; // Option
	VkResult r = CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger);

	if (r != VK_SUCCESS) {
		throw std::exception("failed to set up debug call back");
	}
}

void VulkanApplication::CreateSurface() {
	if (mHWND != nullptr)
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = mHWND;
		createInfo.hinstance = mHINSTANCE;

		if (vkCreateWin32SurfaceKHR(mInstance, &createInfo, nullptr, &mSurface) != VK_SUCCESS) {
			throw std::exception("failed to create window surface!");
		}
	}
	else
	{
		if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS) {
			throw std::exception("failed to create window surface!");
		}
	}
}
void VulkanApplication::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::exception("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (IsDeviceSuitable(device)) {
			mPhysicalDevice = device;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE) {
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

	return indices.isComplete(mQueueFlags) && extensionsSupported&& swapChainAdequate&& supportedFeatures.samplerAnisotropy;

}

VulkanApplication::Familys VulkanApplication::FindQueueFamilies(VkPhysicalDevice device) {
	Familys indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (size_t i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueCount > 0 && (mQueueFlags & QUEUE_GRAPHICS_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		if (queueFamilies[i].queueCount > 0 && (mQueueFlags & QUEUE_COMPUTE_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			indices.computeFamily = i;
		}
		if (queueFamilies[i].queueCount > 0 && (mQueueFlags & QUEUE_TRANSFER_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			indices.transferFamily = i;
		}
		if (queueFamilies[i].queueCount > 0 && (mQueueFlags & QUEUE_SPARSE_BINDING_BIT) && queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			indices.sparseBindingFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}
		if (indices.isComplete(mQueueFlags))
		{
			return indices;
		}
	}

	return indices;
}

VulkanApplication::SwapChainSupportDetails VulkanApplication::QuerySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool VulkanApplication::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void VulkanApplication::CreateDevice() {

	Familys indices = FindQueueFamilies(mPhysicalDevice);
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

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

	if (mEnableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
		createInfo.ppEnabledLayerNames = mValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicDevice) != VK_SUCCESS) {
		throw std::exception("failed to create logical device!");
	}

	GetQueues(indices);

}

void VulkanApplication::GetQueues(Familys queueFamily) {
	if (mQueueFlags & QUEUE_GRAPHICS_BIT)
	{
		vkGetDeviceQueue(mLogicDevice, queueFamily.graphicsFamily.value(), 0, &mQueues.graphicsQueue);
	}

	if (mQueueFlags & QUEUE_COMPUTE_BIT)
	{
		vkGetDeviceQueue(mLogicDevice, queueFamily.computeFamily.value(), 0, &mQueues.computeQueue);
	}

	if (mQueueFlags & QUEUE_TRANSFER_BIT)
	{
		vkGetDeviceQueue(mLogicDevice, queueFamily.transferFamily.value(), 0, &mQueues.transferQueue);
	}

	if (mQueueFlags & QUEUE_SPARSE_BINDING_BIT)
	{
		vkGetDeviceQueue(mLogicDevice, queueFamily.sparseBindingFamily.value(), 0, &mQueues.sparseBindingQueue);
	}

	vkGetDeviceQueue(mLogicDevice, queueFamily.presentFamily.value(), 0, &mQueues.presentQueue);
}

void VulkanApplication::CreateSwapChain() {
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	Familys indices = FindQueueFamilies(mPhysicalDevice);
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
	VkResult r = vkCreateSwapchainKHR(mLogicDevice, &createInfo, nullptr, &mSwapChain);
	if (r != VK_SUCCESS) {
		throw std::exception("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(mLogicDevice, mSwapChain, &imageCount, nullptr);
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mLogicDevice, mSwapChain, &imageCount, mSwapChainImages.data());
	mSwapChainImageFormat = surfaceFormat.format;
	mSwapChainExtent = extent;
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
		glfwGetFramebufferSize(mWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		return actualExtent;
	}

}


void VulkanApplication::CreateImageViews() {
	mSwapChainImageViews.resize(mSwapChainImages.size());

	for (uint32_t i = 0; i < mSwapChainImages.size(); i++) {
		mSwapChainImageViews[i] = CreateImageView(mSwapChainImages[i], mSwapChainImageFormat);
	}
}


VkImageView VulkanApplication::CreateImageView(VkImage image, VkFormat format) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(mLogicDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::exception("failed to create texture image view!");
	}

	return imageView;
}

void VulkanApplication::CreateRenderPass() {
	
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = mSwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;



	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(mLogicDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
		throw std::exception("failed to create render pass!");
	}
	mRenderPasses["First"] = mRenderPass;
}

void VulkanApplication::CreateGraphicsPipeline() {

	//Pipeline one
	{
		//system("compile.bat");
		//off line pipeline
		{
			Shader verte("firstStage.spv", ShaderStage::SHADER_STAGE_VERTEX_BIT);
			Shader frag("firstStage.spv", ShaderStage::SHADER_STAGE_FRAGMENT_BIT);
			std::vector<Shader> shaders = { verte,frag };

			PipelineBindingInfo binding{ 0,sizeof(Vertex_Aki),INPUT_RATE_VERTEX };
			std::vector<PipelineBindingInfo>bindingInfos = { binding };

			std::vector<PipelineAttributeInfo> attributeDescripti(5);

			attributeDescripti[0].binding = 0;
			attributeDescripti[0].location = 0;
			attributeDescripti[0].format = FORMAT_R32G32B32_SFLOAT;
			attributeDescripti[0].offset = offsetof(Vertex_Aki, position);

			attributeDescripti[1].binding = 0;
			attributeDescripti[1].location = 1;
			attributeDescripti[1].format = FORMAT_R32G32B32_SFLOAT;
			attributeDescripti[1].offset = offsetof(Vertex_Aki, normal);

			attributeDescripti[2].binding = 0;
			attributeDescripti[2].location = 2;
			attributeDescripti[2].format = FORMAT_R32G32_SFLOAT;
			attributeDescripti[2].offset = offsetof(Vertex_Aki, texCoord);

			attributeDescripti[3].binding = 0;
			attributeDescripti[3].location = 3;
			attributeDescripti[3].format = FORMAT_R32G32B32_SFLOAT;
			attributeDescripti[3].offset = offsetof(Vertex_Aki, texCoord2);

			attributeDescripti[4].binding = 0;
			attributeDescripti[4].location = 4;
			attributeDescripti[4].format = FORMAT_R32G32B32A32_SFLOAT;
			attributeDescripti[4].offset = offsetof(Vertex_Aki, texCoord3);

			std::vector<PipelineDescriptorSetInfo> desInfo(2);
			desInfo[0].binding = 0;
			desInfo[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			desInfo[0].count = 1;
			desInfo[0].stageFlags = SHADER_STAGE_VERTEX_BIT;

			desInfo[1].binding = 1;
			desInfo[1].type = DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desInfo[1].count = 1;
			desInfo[1].stageFlags = SHADER_STAGE_FRAGMENT_BIT;
		
			Pipeline newPipeline({ verte,frag }, { binding }, attributeDescripti, desInfo, TOPOLOGY_TRIANGLE_LIST, Rect{ 0,0,800,800 }, IntRect{ 0,0,800,800 });

			mPipelineLayouts[mRenderPass].push_back(newPipeline.mPipelineLayout);
			mPipelines[mRenderPass].push_back(newPipeline.mPipeline);

			for (int i = 0; i < shaders.size(); i++)
			{
				shaders[i].Destroy();
			}
		}
		//deffer pipeline
		{
			Shader verte("secondStage.spv", ShaderStage::SHADER_STAGE_VERTEX_BIT);
			Shader frag("secondStage.spv", ShaderStage::SHADER_STAGE_FRAGMENT_BIT);
			std::vector<Shader> shaders = { verte,frag };

			PipelineBindingInfo binding{ 0,sizeof(glm::vec3),INPUT_RATE_VERTEX };
			std::vector<PipelineBindingInfo>bindingInfos = { binding };

			std::vector<PipelineAttributeInfo> attributeDescripti(1);

			attributeDescripti[0].binding = 0;
			attributeDescripti[0].location = 0;
			attributeDescripti[0].format = FORMAT_R32G32B32_SFLOAT;
			attributeDescripti[0].offset = 0;

			std::vector<PipelineDescriptorSetInfo> desInfo(4);
			//light info
			desInfo[0].binding = 0;
			desInfo[0].type = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			desInfo[0].count = 1;
			desInfo[0].stageFlags = SHADER_STAGE_VERTEX_BIT;
			//position image
			desInfo[1].binding = 1;
			desInfo[1].type = DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desInfo[1].count = 1;
			desInfo[1].stageFlags = SHADER_STAGE_FRAGMENT_BIT;
			//mormal image
			desInfo[2].binding = 2;
			desInfo[2].type = DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desInfo[2].count = 1;
			desInfo[2].stageFlags = SHADER_STAGE_FRAGMENT_BIT;
			//color image
			desInfo[2].binding = 2;
			desInfo[2].type = DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desInfo[2].count = 1;
			desInfo[2].stageFlags = SHADER_STAGE_FRAGMENT_BIT;

			Pipeline newPipeline({ verte,frag }, { binding }, attributeDescripti, desInfo, TOPOLOGY_TRIANGLE_LIST, Rect{ 0,0,800,800 }, IntRect{ 0,0,800,800 });

			mPipelineLayouts[mRenderPass].push_back(newPipeline.mPipelineLayout);
			mPipelines[mRenderPass].push_back(newPipeline.mPipeline);

			for (int i = 0; i < shaders.size(); i++)
			{
				shaders[i].Destroy();
			}		
		}
	}
}

VkShaderModule VulkanApplication::CreateShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();

	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(mLogicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::exception("failed to create shader module!");
	}
	return shaderModule;

}


void VulkanApplication::CreateFramebuffers() {





	VkImageView attachments[2] = {
		
	};

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = mRenderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = attachments;
	framebufferInfo.width = mSwapChainExtent.width;
	framebufferInfo.height = mSwapChainExtent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(mLogicDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffer) != VK_SUCCESS) {
		throw std::exception("failed to create framebuffer!");
	}

}


void VulkanApplication::CreateCommandPool() {
	Familys queueFamilyIndices = FindQueueFamilies(mPhysicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0; // Optional
	if (vkCreateCommandPool(mLogicDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
		throw std::exception("failed to create command pool!");
	}
}

void VulkanApplication::CreateCommandBuffers() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(mLogicDevice, &allocInfo, &mCommandBuffer) != VK_SUCCESS) {
		throw std::exception("failed to allocate command buffers!");
	}


	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	vkBeginCommandBuffer(mCommandBuffer, &beginInfo);

	for (auto pass : mRenderPasses)
	{

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pass.second;
		renderPassInfo.framebuffer = mSwapChainFramebuffer;

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = mSwapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines[pass.second][0]);

		VkViewport mviewport = {};
		mviewport.x = 0;
		mviewport.y = 0;
		mviewport.width = mSwapChainExtent.width;
		mviewport.height = mSwapChainExtent.height;
		mviewport.minDepth = 0.0f;
		mviewport.maxDepth = 1.0f;

		VkRect2D mscissor = {};
		mscissor.offset = { 0,0 };
		mscissor.extent = mSwapChainExtent;
		vkCmdSetViewport(mCommandBuffer, 0, 1, &mviewport);
		vkCmdSetScissor(mCommandBuffer, 0, 1, &mscissor);
		VkBuffer vertexBuffers[] = { mVertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		for (int j = 0; j < mRenderPass_Meshes[pass.second].size(); j++)
		{


			vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &mRenderPass_Meshes[pass.second][j]->mVertexBuffer, offsets);
			vkCmdBindIndexBuffer(mCommandBuffer, mRenderPass_Meshes[pass.second][j]->mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayouts[pass.second][0], 0, 1, &mDescriptorSets, 0, nullptr);


			vkCmdDrawIndexed(mCommandBuffer, mRenderPass_Meshes[pass.second][j]->mIndicies.size(), 1, 0, 0, 0);

		}

		vkCmdEndRenderPass(mCommandBuffer);

	}



	if (vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS) {
		throw std::exception("failed to record command buffer!");
	}

}

void VulkanApplication::CreateSyncObjects() {
	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(mLogicDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(mLogicDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(mLogicDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

			throw std::exception("failed to create synchronization objects for a frame!");
		}
	}
}

void VulkanApplication::RecreateSwapChain() {
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(mWindow, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(mLogicDevice);
	CleanupSwapChain();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateCommandBuffers();

}


void VulkanApplication::UpdateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	UniformBufferObject ubo = {};
	ubo.view = glm::lookAt(glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(30.f), mSwapChainExtent.width / (float)mSwapChainExtent.height, 0.1f, 10.0f);
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj[1][1] *= -1;
	void* data;
	vkMapMemory(mLogicDevice, mUniformBuffersMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mLogicDevice, mUniformBuffersMemory);



}

void VulkanApplication::CreateTextureImage(const char* filePath) {
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::exception("failed to load texture image!");
	}
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(mLogicDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(mLogicDevice, stagingBufferMemory);
	stbi_image_free(pixels);

	CreateImage(texWidth, texHeight,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mTextureImage, mTextureImageMemory);
	TransitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(stagingBuffer, mTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	TransitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


}
void VulkanApplication::CreateTextureImageView() {
	mTextureImageView = CreateImageView(mTextureImage, VK_FORMAT_R8G8B8A8_UNORM);

}
void VulkanApplication::CreateTextureSampler() {
	{

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
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		if (vkCreateSampler(mLogicDevice, &samplerInfo, nullptr, &mTextureSampler[0]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	{

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_TRUE;
		samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1.0f;
		if (vkCreateSampler(mLogicDevice, &samplerInfo, nullptr, &mTextureSampler[1]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

}


void VulkanApplication::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(mLogicDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(mLogicDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mLogicDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(mLogicDevice, buffer, bufferMemory, 0);
}


uint32_t VulkanApplication::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::exception("failed to find suitable memory type!");
}


void VulkanApplication::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(mLogicDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(mLogicDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mLogicDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(mLogicDevice, image, imageMemory, 0);
}

void VulkanApplication::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	EndSingleTimeCommands(commandBuffer);
}

void VulkanApplication::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	EndSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanApplication::BeginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(mLogicDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanApplication::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(mQueues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(mQueues.graphicsQueue);

	vkFreeCommandBuffers(mLogicDevice, mCommandPool, 1, &commandBuffer);
}


template<class T>
void VulkanApplication::CreateVertexBuffer(std::vector<T>vertexdata, VkRenderPass renderpass) {

	VkDeviceSize bufferSize = sizeof(vertexdata[0]) * vertexdata.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(mLogicDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertexdata.data(), (size_t)bufferSize);
	vkUnmapMemory(mLogicDevice, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

	CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);
	//	mVertexBuffers[renderpass].push_back(mVertexBuffer);

	vkDestroyBuffer(mLogicDevice, stagingBuffer, nullptr);
	vkFreeMemory(mLogicDevice, stagingBufferMemory, nullptr);
}


void VulkanApplication::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	EndSingleTimeCommands(commandBuffer);
}


void VulkanApplication::UploadMesh(std::string renderPass, Mesh * mesh, int submesh) {
	mRenderPass_Meshes[mRenderPasses[renderPass]].push_back(mesh);

}

void  VulkanApplication::CreateUniformBuffers() {
	VkDeviceSize bufferSize1 = sizeof(UniformBufferObject) + sizeof(LightInfo);

	CreateBuffer(bufferSize1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers, mUniformBuffersMemory);
}

void VulkanApplication::CreateDescriptorPool() {
	std::array<VkDescriptorPoolSize, 3> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[2].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(mSwapChainImages.size());

	if (vkCreateDescriptorPool(mLogicDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

}

void VulkanApplication::CreateDescriptorSets() {

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = mDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &mDescriptorSetLayout;
	//TODO
	if (vkAllocateDescriptorSets(mLogicDevice, &allocInfo, &mDescriptorSets) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	//off scene
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = mUniformBuffers;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mTextureImageView;
	imageInfo.sampler = mTextureSampler[0];

	VkDescriptorBufferInfo bufferInfo2 = {};
	bufferInfo2.buffer = mUniformBuffers;
	bufferInfo2.offset = sizeof(UniformBufferObject);
	bufferInfo2.range = sizeof(LightInfo);

	std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = mDescriptorSets;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = mDescriptorSets;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = mDescriptorSets;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pBufferInfo = &bufferInfo2;

	vkUpdateDescriptorSets(mLogicDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);


	LightInfo light;
	light.ambientStrenth = 0.1f;
	light.lightColor = glm::vec3(1.0f,1.0f, 1.0f);
	light.lightPos = glm::vec3(2000.0f, 2000.0f, 2000.0f);
	void* data;
	VkResult r = vkMapMemory(mLogicDevice, mUniformBuffersMemory, sizeof(UniformBufferObject), sizeof(LightInfo), 0, &data);
	memcpy(data, &light, sizeof(LightInfo));
	vkUnmapMemory(mLogicDevice, mUniformBuffersMemory);

}


void VulkanApplication::CreateDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding vertexUniformBinding = {};
	vertexUniformBinding.binding = 0;
	vertexUniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vertexUniformBinding.descriptorCount = 1;
	vertexUniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vertexUniformBinding.pImmutableSamplers = nullptr; // Optional
	//color
	VkDescriptorSetLayoutBinding sampler1LayoutBinding = {};
	sampler1LayoutBinding.binding = 1;
	sampler1LayoutBinding.descriptorCount = 1;
	sampler1LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler1LayoutBinding.pImmutableSamplers = nullptr;
	sampler1LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	//normal
	VkDescriptorSetLayoutBinding sampler2LayoutBinding = {};
	sampler2LayoutBinding.binding = 1;
	sampler2LayoutBinding.descriptorCount = 1;
	sampler2LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler2LayoutBinding.pImmutableSamplers = nullptr;
	sampler2LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	//position
	VkDescriptorSetLayoutBinding sampler3LayoutBinding = {};
	sampler3LayoutBinding.binding = 1;
	sampler3LayoutBinding.descriptorCount = 1;
	sampler3LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	sampler3LayoutBinding.pImmutableSamplers = nullptr;
	sampler3LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding fragmentUniformBinding = {};
	fragmentUniformBinding.binding = 2;
	fragmentUniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	fragmentUniformBinding.descriptorCount = 1;
	fragmentUniformBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentUniformBinding.pImmutableSamplers = nullptr; // Optional


	std::array<VkDescriptorSetLayoutBinding, 5> bindings = { vertexUniformBinding, sampler1LayoutBinding,sampler2LayoutBinding,sampler3LayoutBinding,fragmentUniformBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(mLogicDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

}

//help functions
VkInstance VulkanApplication::GetInstance() {
	return mInstance;
}
VkDevice VulkanApplication::GetDevice() {
	return mLogicDevice;
}

VkRenderPass VulkanApplication::GetRenderPass() {
	return mRenderPass;
}

VkExtent2D VulkanApplication::GetSwipchainExtent() {
	return mSwapChainExtent;
}

//vulkan functions
void VulkanApplication::MainLoop() {
	int i = 0;
	while (!glfwWindowShouldClose(mWindow)) {
		ProcessInput(mWindow);		
		CreateCommandBuffers();
		glfwPollEvents();
		DrawFrame();
		++i;
		i = i % 3;
	}

	vkDeviceWaitIdle(mLogicDevice);
}


void VulkanApplication::Cleanup() {


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(mLogicDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(mLogicDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(mLogicDevice, mInFlightFences[i], nullptr);
	}
	//vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	//vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
	//cleanupSwapChain();
	vkDestroySampler(mLogicDevice, mTextureSampler[0], nullptr);
	vkDestroySampler(mLogicDevice, mTextureSampler[1], nullptr);
	vkDestroyImageView(mLogicDevice, mTextureImageView, nullptr);

	vkDestroyImage(mLogicDevice, mTextureImage, nullptr);
	vkFreeMemory(mLogicDevice, mTextureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(mLogicDevice, mDescriptorSetLayout, nullptr);
	vkDestroyBuffer(mLogicDevice, mIndexBuffer, nullptr);
	vkFreeMemory(mLogicDevice, mIndexBufferMemory, nullptr);

	vkDestroyBuffer(mLogicDevice, mVertexBuffer, nullptr);
	vkFreeMemory(mLogicDevice, mVertexBufferMemory, nullptr);

	vkDestroyCommandPool(mLogicDevice, mCommandPool, nullptr);


	vkDestroyDevice(mLogicDevice, nullptr);
	//DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
	glfwDestroyWindow(mWindow);

	glfwTerminate();
}



void VulkanApplication::CleanupSwapChain() {
	vkFreeCommandBuffers(mLogicDevice, mCommandPool, 1, &mCommandBuffer);


	vkDestroyFramebuffer(mLogicDevice, mSwapChainFramebuffer, nullptr);


	vkDestroyPipeline(mLogicDevice, mPipeline, nullptr);
	vkDestroyPipelineLayout(mLogicDevice, mPipelineLayout, nullptr);
	vkDestroyRenderPass(mLogicDevice, mRenderPass, nullptr);
	for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
		vkDestroyImageView(mLogicDevice, mSwapChainImageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(mLogicDevice, mSwapChain, nullptr);

	
		vkDestroyBuffer(mLogicDevice, mUniformBuffers, nullptr);
		vkFreeMemory(mLogicDevice, mUniformBuffersMemory, nullptr);
	
	vkDestroyDescriptorPool(mLogicDevice, mDescriptorPool, nullptr);


}


void VulkanApplication::DrawFrame() {

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	VkCommandBuffer commandBuffer;
	if (vkAllocateCommandBuffers(mLogicDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::exception("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	for (auto pass : mRenderPasses)
	{

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pass.second;
		renderPassInfo.framebuffer = mSwapChainFramebuffer;

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = mSwapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines[pass.second][0]);

		VkViewport mviewport = {};
		mviewport.x = 0;
		mviewport.y = 0;
		mviewport.width = mSwapChainExtent.width;
		mviewport.height = mSwapChainExtent.height;
		mviewport.minDepth = 0.0f;
		mviewport.maxDepth = 1.0f;

		VkRect2D mscissor = {};
		mscissor.offset = { 0,0 };
		mscissor.extent = mSwapChainExtent;
		vkCmdSetViewport(commandBuffer, 0, 1, &mviewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &mscissor);
		VkBuffer vertexBuffers[] = { mVertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		for (int j = 0; j < mRenderPass_Meshes[pass.second].size(); j++)
		{


			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mRenderPass_Meshes[pass.second][j]->mVertexBuffer, offsets);
			vkCmdBindIndexBuffer(commandBuffer, mRenderPass_Meshes[pass.second][j]->mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayouts[pass.second][0], 0, 1, &mDescriptorSets, 0, nullptr);


			vkCmdDrawIndexed(commandBuffer, mRenderPass_Meshes[pass.second][j]->mIndicies.size(), 1, 0, 0, 0);

		}

		vkCmdEndRenderPass(commandBuffer);

	}



	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::exception("failed to record command buffer!");
	}

	vkWaitForFences(mLogicDevice, 1, &mInFlightFences[currentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(mLogicDevice, mSwapChain, (std::numeric_limits<uint64_t>::max)(), mImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::exception("failed to acquire swap chain image!");
	}

	UpdateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(mLogicDevice, 1, &mInFlightFences[currentFrame]);


	if (vkQueueSubmit(mQueues.graphicsQueue, 1, &submitInfo, mInFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::exception("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { mSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	result = vkQueuePresentKHR(mQueues.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
		mFramebufferResized = false;
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::exception("failed to present swap chain image!");
	}

	vkQueueWaitIdle(mQueues.presentQueue);
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}