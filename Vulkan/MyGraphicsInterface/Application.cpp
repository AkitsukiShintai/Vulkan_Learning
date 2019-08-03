//#include"Application.h"
#include"VulkanApplication.h"
Application* Application::m_Application = nullptr;


Application::Application(std::string _applicationName, int _width, int _height)
	:m_ApplicationName(_applicationName)
	, m_Width(_width)
	, m_Height(_height)
{
	
};

Application::Application() :m_ApplicationName("")
, m_Width(0)
, m_Height(0) {

}

Application*  Application::GetInstance() {
	if (m_Application == nullptr)
	{
		m_Application = new VulkanApplication();
	}

	return m_Application;
}
void Application::SetResizable(bool state) {


}
void Application::Init() {
	if (m_Application == nullptr)
	{
		m_Application = new VulkanApplication();
	}
#ifdef __VULKAN_APPLICATION
	app = static_cast<VulkanApplication*>(m_Application);
	app->Init();
#endif // __VULKAN_APPLICATION


}

void Application::SetApplicationInfo(std::string applicationName, int width, int height) {
	
}

void Application::Run() {
}



uint32_t Application::CreateImageTexture(std::string path) {
	uint32_t width, height;
	void* data;
	//m_Images.Get() = app->CreateTextureImage(path.c_str());
	//return m_Images.GetIndex();
	return 0;
}


void Application::UploadMesh(std::string pass, Mesh* m_esh, int submesh ) {
	if (pass == "opaque")
	{
		app->UploadOpaqueMesh(m_esh, submesh);
	}
};