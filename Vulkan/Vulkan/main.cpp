//#define VK_USE_PLATFORM_WIN32_KHR
//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_INCLUDE_VULKAN
//#include <glfw3.h>
//#define GLFW_INCLUDE_VULKAN
//#include <glfw3native.h>
//#include <iostream>
//#include <stdexcept>
//#include <functional>
//#include <cstdlib>
//#include<vector>
//#include<algorithm>
//#include<string>
//#include<assert.h>
//#include<optional>
//#include <vulkan/vulkan_core.h>
//#include <windows.h>
//#include <set>
//#include<limits>
//#include <fstream>
//struct QueueFamilyIndices {
//	std::optional<uint32_t> graphicsFamily;
//	std::optional<uint32_t> presentFamily;
//
//	bool isComplete() {
//		return graphicsFamily.has_value() && presentFamily.has_value();
//	}
//};
//
//struct SwapChainSupportDetails {
//	VkSurfaceCapabilitiesKHR capabilities;
//	std::vector<VkSurfaceFormatKHR> formats;
//	std::vector<VkPresentModeKHR> presentModes;
//};
//
//static std::vector<char> readFile(const std::string& filename) {
//	std::ifstream file(filename, std::ios::ate | std::ios::binary);
//
//	if (!file.is_open()) {
//		throw std::exception("failed to open file!");
//	}
//
//	size_t fileSize = (size_t)file.tellg();
//	std::vector<char> buffer(fileSize);
//	file.seekg(0);
//	file.read(buffer.data(), fileSize);
//	file.close();
//
//	return buffer;
//
//}
//
//class HelloTriangleApplication {
//public:
//	void run() {
//		
//		initWindow();
//		initVulkan();
//		mainLoop();
//		cleanup();
//	}
//	const int WIDTH = 800;
//	const int HEIGHT = 600;
//
//	const std::vector<const char*> validationLayers = {
//	"VK_LAYER_LUNARG_standard_validation"
//	};
//
//#ifdef NDEBUG
//	const bool enableValidationLayers = false;
//#else
//	const bool enableValidationLayers = true;
//#endif
//private:
//	GLFWwindow* window;
//	VkInstance instance;
//	VkDebugUtilsMessengerEXT debugMessenger;
//	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
//	VkDevice device;
//	VkQueue graphicsQueue;
//	VkSurfaceKHR surface;
//	VkQueue presentQueue;
//	VkSwapchainKHR swapChain;
//	std::vector<VkImage> swapChainImages;
//	VkFormat swapChainImageFormat;
//	VkExtent2D swapChainExtent;
//	std::vector<VkImageView> swapChainImageViews;
//	VkPipelineLayout pipelineLayout;
//	VkRenderPass renderPass;
//	VkPipeline graphicsPipeline;
//	std::vector<VkFramebuffer> swapChainFramebuffers;
//	VkCommandPool commandPool;
//	std::vector<VkCommandBuffer> commandBuffers;
//	const int MAX_FRAMES_IN_FLIGHT = 2;
//	std::vector<VkSemaphore> imageAvailableSemaphores;
//	std::vector<VkSemaphore> renderFinishedSemaphores;
//	VkSemaphore imageAvailableSemaphore;
//	VkSemaphore renderFinishedSemaphore;
//	std::vector<VkFence> inFlightFences;
//	size_t currentFrame = 0;
//
//
//	const std::vector<const char*> deviceExtensions = {
//		VK_KHR_SWAPCHAIN_EXTENSION_NAME
//	};
//
//
//
//	void initWindow() {
//		glfwInit();
//
//		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//
//		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
//	}
//	void initVulkan() {
//		createInstance();
//		setupDebugMessenger();
//		createSurface();
//		pickPhysicalDevice();
//		createLogicalDevice();
//		createSwapChain();
//		createImageViews();
//		createRenderPass();
//		createGraphicsPipeline();
//		createFramebuffers();
//		createCommandPool();
//		createCommandBuffers();
//		createSyncObjects();
//	}
//
//	void mainLoop() {
//		while (!glfwWindowShouldClose(window)) {
//			glfwPollEvents();
//			drawFrame();
//		}
//		vkDeviceWaitIdle(device);
//	}
//
//	void cleanup() {
//		//cleanupSwapChain();
//		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
//		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
//			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
//			vkDestroyFence(device, inFlightFences[i], nullptr);
//		}
//
//		vkDestroyCommandPool(device, commandPool, nullptr);
//
//		for (auto framebuffer : swapChainFramebuffers) {
//			vkDestroyFramebuffer(device, framebuffer, nullptr);
//		}
//
//		vkDestroyPipeline(device, graphicsPipeline, nullptr);
//		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
//		vkDestroyRenderPass(device, renderPass, nullptr);
//
//		for (auto imageView : swapChainImageViews) {
//			vkDestroyImageView(device, imageView, nullptr);
//		}
//
//		vkDestroySwapchainKHR(device, swapChain, nullptr);
//		vkDestroyDevice(device, nullptr);
//
//		if (enableValidationLayers) {
//			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
//		}
//
//		vkDestroySurfaceKHR(instance, surface, nullptr);
//		vkDestroyInstance(instance, nullptr);
//
//		glfwDestroyWindow(window);
//
//		glfwTerminate();
//	}
//
//	void createInstance() {
//
//		if (enableValidationLayers && !checkValidationLayerSupport()) {
//			throw std::exception("validation layers requested, but not available!");
//		}
//
//		VkApplicationInfo appInfo = {};
//		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//		appInfo.pApplicationName = "Hello Triangle";
//		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//		appInfo.pEngineName = "No Engine";
//		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//		appInfo.apiVersion = VK_API_VERSION_1_0;
//		appInfo.pNext = nullptr;
//	
//		VkInstanceCreateInfo createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//		createInfo.pApplicationInfo = &appInfo;
//
//		uint32_t glfwExtensionCount = 0;
//		const char** glfwExtensions;
//		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//
//		createInfo.enabledExtensionCount = glfwExtensionCount;
//		createInfo.ppEnabledExtensionNames = glfwExtensions;
//		createInfo.enabledLayerCount = 0;
//
//		auto require_extensions = getRequiredExtensions();
//		createInfo.enabledExtensionCount = static_cast<uint32_t>(require_extensions.size());
//		createInfo.ppEnabledExtensionNames = require_extensions.data();
//
//
//		//VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
//
//		
//
//		if (enableValidationLayers) {
//			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//			createInfo.ppEnabledLayerNames = validationLayers.data();
//		}
//		else {
//			createInfo.enabledLayerCount = 0;
//		}
//		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
//			throw std::exception("failed to create instance!");
//		}
//		
//		uint32_t extensionCount = 0;
//		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
//		std::vector<VkExtensionProperties> extensions(extensionCount);
//		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
//
//		std::cout << "available extensions:" << std::endl;
//
//		for (const auto& extension : extensions) {
//			std::cout << "\t" << extension.extensionName << std::endl;
//		}
//
//		for (size_t i = 0; i < glfwExtensionCount; i++)
//		{
//			const char* temp = glfwExtensions[i];
//			auto index = std::find_if(extensions.begin(), extensions.end(), [temp](const VkExtensionProperties& a) {
//				if (std::string(a.extensionName) == std::string(temp))
//					return true;
//				return false;
//			});
//			if (index == extensions.end())
//			{
//				std::cout << "not find " << glfwExtensions[i] << " in extensions list." << std::endl;
//			}
//		}
//
//	}
//
//	bool checkValidationLayerSupport() {
//		uint32_t layerCount;
//		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
//
//		std::vector<VkLayerProperties> availableLayers(layerCount);
//		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
//
//		for (const char* layerName : validationLayers) {
//			bool layerFound = false;
//
//			for (const auto& layerProperties : availableLayers) {
//				if (strcmp(layerName, layerProperties.layerName) == 0) {
//					layerFound = true;
//					break;
//				}
//			}
//
//			if (!layerFound) {
//				return false;
//			}
//		}
//
//		return true;
//	}
//
//	std::vector<const char*> getRequiredExtensions() {
//		uint32_t glfwExtensionCount = 0;
//		const char** glfwExtensions;
//		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//
//		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
//
//		if (enableValidationLayers) {
//			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//		}
//
//		return extensions;
//	}
//
//	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//		VkDebugUtilsMessageTypeFlagsEXT messageType,
//		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//		void* pUserData) {
//		
//		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
//			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
//			assert(false);
//		}
//		return VK_FALSE;
//	}
//
//	void setupDebugMessenger() {
//		if (!enableValidationLayers) return;
//		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
//		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//		createInfo.pfnUserCallback = debugCallback;
//		createInfo.pUserData = nullptr; // Optiona
//		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
//			throw std::exception("failed to set up debug messenger!");
//		}
//	}
//
//
//	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
//		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//		if (func != nullptr) {
//			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//		}
//		else {
//			return VK_ERROR_EXTENSION_NOT_PRESENT;
//		}
//	}
//
//	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
//		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//		if (func != nullptr) {
//			func(instance, debugMessenger, pAllocator);
//		}
//	}
//
//	void pickPhysicalDevice() {
//		uint32_t deviceCount = 0;
//		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//		if (deviceCount == 0) {
//			throw std::exception("failed to find GPUs with Vulkan support!");
//		}
//
//
//		std::vector<VkPhysicalDevice> devices(deviceCount);
//		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//		for (const auto& device : devices) {
//			if (isDeviceSuitable(device)) {
//				physicalDevice = device;
//				break;
//			}
//		}
//
//		if (physicalDevice == VK_NULL_HANDLE) {
//			throw std::exception("failed to find a suitable GPU!");
//		}
//
//	}
//
//
//	bool isDeviceSuitable(VkPhysicalDevice device) {
//		
//		QueueFamilyIndices indices = findQueueFamilies(device);
//
//		bool extensionsSupported = checkDeviceExtensionSupport(device);
//
//		bool swapChainAdequate = false;
//		if (extensionsSupported) {
//			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
//			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//		}
//
//
//		return indices.isComplete()&& extensionsSupported && swapChainAdequate;
//	}
//
//	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
//		QueueFamilyIndices indices;
//		uint32_t queueFamilyCount = 0;
//		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//
//		int i = 0;
//		for (const auto& queueFamily : queueFamilies) {
//			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//				indices.graphicsFamily = i;
//			}
//			//check if the queue support our surface
//			VkBool32 presentSupport = false;
//			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
//
//			if (queueFamily.queueCount > 0 && presentSupport) {
//				indices.presentFamily = i;
//			}
//			if (indices.isComplete()) {
//				break;
//			}
//
//			i++;
//		}
//			return indices;
//	}
//
//
//	void createLogicalDevice() {
//		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//
//		//create presentation queue
//		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//
//		float queuePriority = 1.0f;
//		for (uint32_t queueFamily : uniqueQueueFamilies) {
//			VkDeviceQueueCreateInfo queueCreateInfo = {};
//			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//			queueCreateInfo.queueFamilyIndex = queueFamily;
//			queueCreateInfo.queueCount = 1;
//			queueCreateInfo.pQueuePriorities = &queuePriority;
//			queueCreateInfos.push_back(queueCreateInfo);
//		}
//
//		VkPhysicalDeviceFeatures deviceFeatures = {};
//
//		VkDeviceCreateInfo createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//		createInfo.pQueueCreateInfos = queueCreateInfos.data();
//
//		createInfo.pEnabledFeatures = &deviceFeatures;
//
//		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
//		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
//		//for old version vulkan sdk
//		if (enableValidationLayers) {
//			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//			createInfo.ppEnabledLayerNames = validationLayers.data();
//		}
//		else {
//			createInfo.enabledLayerCount = 0;
//		}
//
//		//create logical device
//		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
//			throw std::exception("failed to create logical device!");
//		}
//
//		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
//		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
//	}
//
//	void createSurface() {
//		/*if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create window surface!");
//		}*/
//
//		//VkWin32SurfaceCreateInfoKHR  createInfo = {};
//		//createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
//		//createInfo.hwnd = glfwGetWin32Window(window);
//		//createInfo.hinstance = GetModuleHandle(nullptr);
//		//if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
//		//	throw std::exception("failed to create window surface!");
//		//}
//
//		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create window surface!");
//		}
//	}
//
//
//	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
//		uint32_t extensionCount;
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
//
//		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
//
//		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
//
//		for (const auto& extension : availableExtensions) {
//			requiredExtensions.erase(extension.extensionName);
//		}
//
//		return requiredExtensions.empty();
//	}
//
//
//	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
//		SwapChainSupportDetails details;
//		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
//
//		uint32_t formatCount;
//		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
//
//		if (formatCount != 0) {
//			details.formats.resize(formatCount);
//			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
//		}
//
//		uint32_t presentModeCount;
//		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
//
//		if (presentModeCount != 0) {
//			details.presentModes.resize(presentModeCount);
//			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
//		}
//
//		return details;
//	}
//
//
//	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
//		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
//			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
//		}
//
//		for (const auto& availableFormat : availableFormats) {
//			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//				return availableFormat;
//			}
//		}
//
//		return availableFormats[0];
//	}
//
//
//	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
//		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
//		for (const auto& availablePresentMode : availablePresentModes) {
//			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
//				return availablePresentMode;
//			}
//			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
//				bestMode = availablePresentMode;
//			}
//		}
//
//		return bestMode;
//	}
//
//	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
//		if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
//			return capabilities.currentExtent;
//		}
//		else {
//			VkExtent2D actualExtent = { WIDTH, HEIGHT };
//
//			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
//
//			return actualExtent;
//		}
//	}
//
//
//
//	void createSwapChain() {
//		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
//
//		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
//		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
//		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
//
//		uint32_t imageCount = swapChainSupport.capabilities.minImageCount+1;
//
//		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
//			imageCount = swapChainSupport.capabilities.maxImageCount;
//		}
//
//		VkSwapchainCreateInfoKHR createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//		createInfo.surface = surface;
//		createInfo.minImageCount = imageCount;
//		createInfo.imageFormat = surfaceFormat.format;
//		createInfo.imageColorSpace = surfaceFormat.colorSpace;
//		createInfo.imageExtent = extent;
//		createInfo.imageArrayLayers = 1;
//		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//
//		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//
//		if (indices.graphicsFamily != indices.presentFamily) {
//			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//			createInfo.queueFamilyIndexCount = 2;
//			createInfo.pQueueFamilyIndices = queueFamilyIndices;
//		}
//		else {
//			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//			createInfo.queueFamilyIndexCount = 0; // Optional
//			createInfo.pQueueFamilyIndices = nullptr; // Optional
//		}
//
//		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
//		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//
//		createInfo.presentMode = presentMode;
//		createInfo.clipped = VK_TRUE;
//		createInfo.oldSwapchain = VK_NULL_HANDLE;
//
//
//		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
//			throw std::exception("failed to create swap chain!");
//		}
//
//		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
//		swapChainImages.resize(imageCount);
//		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
//		swapChainImageFormat = surfaceFormat.format;
//		swapChainExtent = extent;
//	}
//
//	void createImageViews() {
//		swapChainImageViews.resize(swapChainImages.size());
//
//		for (size_t i = 0; i < swapChainImages.size(); i++) {
//			VkImageViewCreateInfo createInfo = {};
//			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//			createInfo.image = swapChainImages[i];
//
//			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//			createInfo.format = swapChainImageFormat;
//
//			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//
//			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//			createInfo.subresourceRange.baseMipLevel = 0;
//			createInfo.subresourceRange.levelCount = 1;
//			createInfo.subresourceRange.baseArrayLayer = 0;
//			createInfo.subresourceRange.layerCount = 1;
//
//			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
//				throw std::exception("failed to create image views!");
//			}
//		}
//	}
//
//	void createGraphicsPipeline() {
//		auto vertShaderCode = readFile("vert.spv");
//		auto fragShaderCode = readFile("frag.spv");
//
//		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
//		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
//
//
//		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
//		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//		vertShaderStageInfo.module = vertShaderModule;
//		vertShaderStageInfo.pName = "main";
//		vertShaderStageInfo.pSpecializationInfo = nullptr;
//
//
//		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
//		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//		fragShaderStageInfo.module = fragShaderModule;
//		fragShaderStageInfo.pName = "main";
//
//		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
//
//		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
//		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//		vertexInputInfo.vertexBindingDescriptionCount = 0;
//		vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
//		vertexInputInfo.vertexAttributeDescriptionCount = 0;
//		vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
//
//		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
//		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//		inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//		VkViewport viewport = {};
//		viewport.x = 0.0f;
//		viewport.y = 0.0f;
//		viewport.width = (float)swapChainExtent.width;
//		viewport.height = (float)swapChainExtent.height;
//		viewport.minDepth = 0.0f;
//		viewport.maxDepth = 1.0f;
//
//		VkRect2D scissor = {};
//		scissor.offset = { 0, 0 };
//		scissor.extent = swapChainExtent;
//
//		VkPipelineViewportStateCreateInfo viewportState = {};
//		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//		viewportState.viewportCount = 1;
//		viewportState.pViewports = &viewport;
//		viewportState.scissorCount = 1;
//		viewportState.pScissors = &scissor;
//
//
//		VkPipelineRasterizationStateCreateInfo rasterizer = {};
//		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//		rasterizer.depthClampEnable = VK_FALSE;
//		rasterizer.rasterizerDiscardEnable = VK_FALSE;
//		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//		rasterizer.lineWidth = 1.0f;
//		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//		rasterizer.depthBiasEnable = VK_FALSE;
//		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
//		rasterizer.depthBiasClamp = 0.0f; // Optional
//		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
//
//		VkPipelineMultisampleStateCreateInfo multisampling = {};
//		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//		multisampling.sampleShadingEnable = VK_FALSE;
//		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//		multisampling.minSampleShading = 1.0f; // Optional
//		multisampling.pSampleMask = nullptr; // Optional
//		multisampling.alphaToCoverageEnable = VK_FALSE; // Opt
//		multisampling.alphaToOneEnable = VK_FALSE;
//
//		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
//		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//		colorBlendAttachment.blendEnable = VK_FALSE;
//		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
//		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
//
//		VkPipelineColorBlendStateCreateInfo colorBlending = {};
//		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//		colorBlending.logicOpEnable = VK_FALSE;
//		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
//		colorBlending.attachmentCount = 1;
//		colorBlending.pAttachments = &colorBlendAttachment;
//		colorBlending.blendConstants[0] = 0.0f; // Optional
//		colorBlending.blendConstants[1] = 0.0f; // Optional
//		colorBlending.blendConstants[2] = 0.0f; // Optional
//		colorBlending.blendConstants[3] = 0.0f; // Optional
//
//		VkDynamicState dynamicStates[] = {
//			VK_DYNAMIC_STATE_VIEWPORT,
//			VK_DYNAMIC_STATE_LINE_WIDTH
//		};
//
//		VkPipelineDynamicStateCreateInfo dynamicState = {};
//		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//		dynamicState.dynamicStateCount = 2;
//		dynamicState.pDynamicStates = dynamicStates;
//
//
//		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
//		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//		pipelineLayoutInfo.setLayoutCount = 0; // Optional
//		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
//		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
//		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
//
//		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
//			throw std::exception("failed to create pipeline layout!");
//		}
//
//		
//		VkGraphicsPipelineCreateInfo pipelineInfo = {};
//		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//		pipelineInfo.stageCount = 2;
//		pipelineInfo.pStages = shaderStages;
//		pipelineInfo.pVertexInputState = &vertexInputInfo;
//		pipelineInfo.pInputAssemblyState = &inputAssembly;
//		pipelineInfo.pViewportState = &viewportState;
//		pipelineInfo.pRasterizationState = &rasterizer;
//		pipelineInfo.pMultisampleState = &multisampling;
//		pipelineInfo.pDepthStencilState = nullptr; // Optional
//		pipelineInfo.pColorBlendState = &colorBlending;
//		pipelineInfo.pDynamicState = nullptr; // Optional
//		pipelineInfo.layout = pipelineLayout;
//		pipelineInfo.renderPass = renderPass;
//		pipelineInfo.subpass = 0;
//		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//		pipelineInfo.basePipelineIndex = -1; // Optional
//
//		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
//			throw std::exception("failed to create graphics pipeline!");
//		}
//
//		vkDestroyShaderModule(device, fragShaderModule, nullptr);
//		vkDestroyShaderModule(device, vertShaderModule, nullptr);
//	}
//
//	void createRenderPass() {
//		VkAttachmentDescription colorAttachment = {};
//		colorAttachment.format = swapChainImageFormat;
//		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//
//		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//
//		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//		VkAttachmentReference colorAttachmentRef = {};
//		colorAttachmentRef.attachment = 0;
//		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//
//		VkSubpassDescription subpass = {};
//		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//		subpass.colorAttachmentCount = 1;
//		subpass.pColorAttachments = &colorAttachmentRef;
//
//		VkRenderPassCreateInfo renderPassInfo = {};
//		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//		renderPassInfo.attachmentCount = 1;
//		renderPassInfo.pAttachments = &colorAttachment;
//		renderPassInfo.subpassCount = 1;
//		renderPassInfo.pSubpasses = &subpass;
//
//		VkSubpassDependency dependency = {};
//		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//		dependency.dstSubpass = 0;
//		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		dependency.srcAccessMask = 0;
//		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//		renderPassInfo.dependencyCount = 1;
//		renderPassInfo.pDependencies = &dependency;
//
//
//		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
//			throw std::exception("failed to create render pass!");
//		}
//	}
//
//
//	VkShaderModule createShaderModule(const std::vector<char>& code) {
//		VkShaderModuleCreateInfo createInfo = {};
//		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//		createInfo.codeSize = code.size();
//		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
//
//		VkShaderModule shaderModule;
//		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
//			throw std::exception("failed to create shader module!");
//		}
//
//		return shaderModule;
//	}
//
//	void createFramebuffers() {
//		swapChainFramebuffers.resize(swapChainImageViews.size());
//
//		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
//			VkImageView attachments[] = {
//				swapChainImageViews[i]
//			};
//
//			VkFramebufferCreateInfo framebufferInfo = {};
//			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//			framebufferInfo.renderPass = renderPass;
//			framebufferInfo.attachmentCount = 1;
//			framebufferInfo.pAttachments = attachments;
//			framebufferInfo.width = swapChainExtent.width;
//			framebufferInfo.height = swapChainExtent.height;
//			framebufferInfo.layers = 1;
//
//			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
//				throw std::exception("failed to create framebuffer!");
//			}
//		}
//	}
//
//
//	void  createCommandPool() {
//		
//		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
//
//		VkCommandPoolCreateInfo poolInfo = {};
//		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
//		poolInfo.flags = 0; // Optional
//
//		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
//			throw std::exception("failed to create command pool!");
//		}
//	}
//
//	void  createCommandBuffers() {
//		commandBuffers.resize(swapChainFramebuffers.size());
//
//		VkCommandBufferAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.commandPool = commandPool;
//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();
//
//		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
//			throw std::exception("failed to allocate command buffers!");
//		}
//
//		for (size_t i = 0; i < commandBuffers.size(); i++) {
//			VkCommandBufferBeginInfo beginInfo = {};
//			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
//			beginInfo.pInheritanceInfo = nullptr; // Optional
//
//			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
//				throw std::exception("failed to begin recording command buffer!");
//			}
//
//			VkRenderPassBeginInfo renderPassInfo = {};
//			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//			renderPassInfo.renderPass = renderPass;
//			renderPassInfo.framebuffer = swapChainFramebuffers[i];
//			renderPassInfo.renderArea.offset = { 0, 0 };
//			renderPassInfo.renderArea.extent = swapChainExtent;
//
//			VkClearValue clearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
//			renderPassInfo.clearValueCount = 1;
//			renderPassInfo.pClearValues = &clearColor;
//			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
//
//			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
//
//			vkCmdEndRenderPass(commandBuffers[i]);
//
//			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
//				throw std::exception("failed to record command buffer!");
//			}
//
//		}
//
//	}
//
//		
//	void drawFrame() {
//
//		//vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
//		//vkResetFences(device, 1, &inFlightFences[currentFrame]);
//		uint32_t imageIndex;
//		VkResult r = vkAcquireNextImageKHR(device, swapChain, (std::numeric_limits<uint64_t>::max)(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
//		if (r != VK_SUCCESS)
//		{
//			return;
//		}
//
//		VkSubmitInfo submitInfo = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//		//VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
//		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore};
//		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
//		submitInfo.waitSemaphoreCount = 1;
//		submitInfo.pWaitSemaphores = waitSemaphores;
//		submitInfo.pWaitDstStageMask = waitStages;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffers[0];
//
//		//VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
//		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore};
//		submitInfo.signalSemaphoreCount = 1;
//		submitInfo.pSignalSemaphores = signalSemaphores;
//		r = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//		if (r != VK_SUCCESS) {
//			throw std::exception("failed to submit draw command buffer!");
//		}
//		
//		VkPresentInfoKHR presentInfo = {};
//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//		presentInfo.waitSemaphoreCount = 1;
//		presentInfo.pWaitSemaphores = signalSemaphores;
//
//		VkSwapchainKHR swapChains[] = { swapChain };
//		presentInfo.swapchainCount = 1;
//		presentInfo.pSwapchains = swapChains;
//		presentInfo.pImageIndices = &imageIndex;
//		presentInfo.pResults = nullptr; // Optional
//	
//		vkQueuePresentKHR(presentQueue, &presentInfo);
//
//		//vkQueueWaitIdle(presentQueue);
//
//		
//		//currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//	}
//
//
//	void createSyncObjects() {
//		VkSemaphoreCreateInfo semaphoreInfo = {};
//		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
//			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
//
//			throw std::runtime_error("failed to create semaphores!");
//		}
//
//
//
//		/*imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
//
//		VkFenceCreateInfo fenceInfo = {};
//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
//				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
//				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
//
//				throw std::runtime_error("failed to create synchronization objects for a frame!");
//			}
//		}*/
//	}
//
//	void cleanupSwapChain() {
//		for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
//			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
//		}
//
//		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
//
//		vkDestroyPipeline(device, graphicsPipeline, nullptr);
//		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
//		vkDestroyRenderPass(device, renderPass, nullptr);
//
//		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
//			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
//		}
//
//		vkDestroySwapchainKHR(device, swapChain, nullptr);
//	}
//
//	void recreateSwapChain() {
//		vkDeviceWaitIdle(device);
//
//		cleanupSwapChain();
//
//		createSwapChain();
//		createImageViews();
//		createRenderPass();
//		createGraphicsPipeline();
//		createFramebuffers();
//		createCommandBuffers();
//	}
//};
//
//int main() {
//	HelloTriangleApplication app;
//	app.run();
//	/*try {
//		
//	}
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//		return EXIT_FAILURE;
//	}*/
//	return EXIT_SUCCESS;
//}