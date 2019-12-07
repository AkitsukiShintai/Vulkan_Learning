#pragma once
#include"Enums.h"
#include <stdint.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm.hpp>
#include <array>
struct Vertex_Aki {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texCoord;
	glm::vec3 texCoord2;
	glm::vec4 texCoord3;
};

struct UniformBufferObject {
	glm::mat4 m_odel;
	glm::mat4 view;
	glm::mat4 proj;
};
struct LightInfo {

	glm::vec3 lightPos;
	float  ambientStrenth;
	glm::vec3 lightColor;
	glm::vec3 lightDir;
	LightType type;
};
struct CameraViewInfo {
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 position;
};

struct Camera {
	float fov;
	float nearClip;
	float farClip;
	float aspect;
	CameraViewInfo viewInfo;
};

struct CameraCorner {
	std::array<glm::vec3, 4> nearCorners;
	std::array<glm::vec3, 4> farCorners;
};


struct  WaterParameter
{
	glm::vec4 waterBaseColor;
	glm::vec2 waterDir;
	float fresnel;
	float refractionBumpScale;
	float specPower;
	float waterSpecularIntensity;
	float waterVisibility;
	float waterVisibilityPower;
	float waterRoughness;
	float sampleLength;
	float waterSpeed;
	float waterHeight;
	float totalTime;
	float waveHeight;
};