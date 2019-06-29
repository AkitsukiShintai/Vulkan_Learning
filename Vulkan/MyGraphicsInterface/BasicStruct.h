#pragma once
#include"Enums.h"
#include <stdint.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm.hpp>
struct Rect {
	float x0, y0;
	float x1, y1;
};

struct IntRect {
	int x0, y0;
	int x1, y1;
};


struct PipelineBindingInfo {
	uint32_t binding;
	uint32_t size;
	VertexInputRate inputRate;
};

struct PipelineAttributeInfo {
	uint32_t binding;
	uint32_t location;
	uint32_t offset;
	DataFormat format;
};

struct PipelineDescriptorSetInfo {
	DescriptorType type;
	int binding;
	int count;
	ShaderStage stageFlags;
};

struct Vertex_Aki {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 texCoord2;
	glm::vec4 texCoord3;
};