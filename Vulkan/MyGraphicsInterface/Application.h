#pragma once
#include "Enums.h"
#include<string>
#include<stdarg.h>
#include<vector>
#include<optional>
#include<Windows.h>
#include"Mesh.h"
#include <glfw3.h>
#include "ObjectPool.h"
class VulkanApplication;
class Application {
protected:
	std::string m_ApplicationName;
	int m_Width;
	int m_Height;
protected:
	//Init Application with window name, the width of window and the height of window
	Application(std::string applicationName, int width, int height);
	Application();
private:
	 static Application* m_Application;
public:
	 static Application* GetInstance();
	  void SetResizable(bool state);
	  virtual void Init();
	  virtual void Run();
	  virtual void SetApplicationInfo(std::string applicationName, int width, int height);
	  void UploadVertexData() {};
	  void UploadIndexData() {};
	  void UploadMesh(std::string pass, Mesh* m_esh, int submesh = 0);
#ifdef __VULKAN_APPLICATION
	  virtual GLFWwindow* GetWindow() { return nullptr; };
	 virtual void SetRequiredValadationLayers(std::vector<const char*> valadationLayers) {};
	 virtual void SetRequiredExtentions(std::vector<const char*> extentions) {};
	 virtual void SetWindowHandle(HWND window, HINSTANCE handle) {};
	 virtual void SetDeviceExtensions(std::vector<const char*> extentions) {};
	 virtual void SetQueueFlag(QueueFlagBits flag) {};
	 virtual void CreatePipeline(RenderPassType renderpass) {};

	 uint32_t CreateRenderPass(std::vector<uint32_t> attachments) { return -1; };
	 uint32_t CreateImageTexture(std::string path);
	 void AttachImageToRenderpass(uint32_t image, uint32_t renderPass) {};


private:
	VulkanApplication* app;
	/*ObjectPool<Image, 200> m_Images;
	ObjectPool<Pipeline, 200> m_Pipelines;
	ObjectPool<VkBuffer, 1000> m_Buffers;
	ObjectPool<VkSampler, 20> m_Samplers;
	ObjectPool<VkFramebuffer, 200> m_Framebuffers;
	ObjectPool<VkDescriptorSet, 1000> m_DescriptorSets;*/









#endif // __VULKAN_APPLICATION


};

