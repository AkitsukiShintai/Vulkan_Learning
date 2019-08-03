#pragma once
#include"Enums.h"
#include <stdint.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm.hpp>
struct Vertex_Aki {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texCoord;
	glm::vec3 texCoord2;
	glm::vec4 texCoord3;
};