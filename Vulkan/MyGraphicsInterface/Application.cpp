//#include"Application.h"
#include"VulkanApplication.h"
Application* Application::mApplication = nullptr;


Application::Application(std::string _applicationName, int _width, int _height)
	:mApplicationName(_applicationName)
	, mWidth(_width)
	, mHeight(_height)
{
	
};

Application::Application() :mApplicationName("")
, mWidth(0)
, mHeight(0) {

}

Application*  Application::GetInstance() {
	if (mApplication == nullptr)
	{
		mApplication = new VulkanApplication();
	}

	return mApplication;
}
void Application::SetResizable(bool state) {


}
void Application::Init() {

}

void Application::SetApplicationInfo(std::string applicationName, int width, int height) {
	
}

void Application::Run() {
}
