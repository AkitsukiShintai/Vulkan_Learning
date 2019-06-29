#pragma once
#include "Enums.h"
#include<string>
#include<stdarg.h>
#include<vector>
#include<optional>
#include<Windows.h>
#include"Mesh.h"
class Application {
protected:
	std::string mApplicationName;
	int mWidth;
	int mHeight;
protected:
	//Init Application with window name, the width of window and the height of window
	Application(std::string applicationName, int width, int height);
	Application();
private:
	 static Application* mApplication;
public:
	 static Application* GetInstance();
	 virtual void SetResizable(bool state);
	 virtual void Init();
	 virtual void Run();
	 virtual void SetApplicationInfo(std::string applicationName, int width, int height);
	 virtual void UploadVertexData() {};
	 virtual void UploadIndexData() {};
	 virtual void UploadMesh(std::string pass, Mesh* mesh, int submesh = 0) {};
#ifdef __VULKAN_APPLICATION
	 virtual void SetRequiredValadationLayers(std::vector<const char*> valadationLayers) {};
	 virtual void SetRequiredExtentions(std::vector<const char*> extentions) {};
	 virtual void SetWindowHandle(HWND window, HINSTANCE handle) {};
	 virtual void SetDeviceExtensions(std::vector<const char*> extentions) {};
	 virtual void SetQueueFlag(QueueFlagBits flag) {};
	 virtual void CreatePipeline(RenderPassType renderpass) {};
#endif // __VULKAN_APPLICATION


};

