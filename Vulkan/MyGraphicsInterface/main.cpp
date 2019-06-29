#define __VULKAN_APPLICATION
#include"Application.h"
#include"Mesh.h"
#include"ModelReader.h"

int main() {
	Application* mApp = Application::GetInstance();

	mApp->SetApplicationInfo("Vulkan",800,800);

	mApp->Init();

	Mesh* mesh = new Mesh();
	ModelReader::ReadModule("bunny.ply", mesh);
	mesh->UploadMesh();
	mApp->UploadMesh("First", mesh);

	mApp->Run();
}