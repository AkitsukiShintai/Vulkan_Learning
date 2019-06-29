#pragma once
#include"VulkanApplication.h"
#include "BasicStruct.h"
#include"Mesh.h"
class CommandBuffer {
public:
	CommandBuffer();
	void SetViewport(Rect viewport);
	//void Draw(Mesh mesh,Material material, int submesh);
};