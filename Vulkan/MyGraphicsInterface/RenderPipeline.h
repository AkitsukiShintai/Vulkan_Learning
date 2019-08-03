//#pragma once
//#include"Enums.h"
//#include "BasicStruct.h"
//#include<map>
//#include<vector>
//#include<string>
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm.hpp>
//#include<vulkan/vulkan.h>
////Pipeline part
//class Shader {
//public:
//	Shader(const Shader& src);
//	Shader(const std::string& shaderPath, const ShaderStage stageBits,const char* mainFunc = "main");
//	~Shader();
//	std::string GetPath();
//	ShaderStage GetStage();
//	void Destroy();
//private:
//	const char* mMainFunc;
//	friend class Pipeline;
//	friend class VulkanApplication;
//	const std::string mPath;
//	ShaderStage mStage;
//	VkShaderModule mModule;
//};
//
//class MaterialPropertyBlock {
//public:
//	MaterialPropertyBlock();
//	void SetVector(int nameID, glm::vec2 value);
//	void SetVector(std::string bindingName, glm::vec2 value);
//	void SetVector(int nameID, glm::vec3 value);
//	void SetVector(std::string bindingName, glm::vec3 value);
//	void SetVector(int nameID, glm::vec4 value);
//	void SetVector(std::string bindingName, glm::vec4 value);
//	void SetMatrix(int nameID, glm::mat4x4 value);
//	void SetMatrix(std::string bindingName, glm::mat4x4 value);
//	void SetColor(int nameID, glm::vec4 value);
//	void SetColor(std::string bindingName, glm::vec4 value);
//
//private:
//	std::map<int, glm::vec2> mID2Vector2Map;
//	std::map<std::string, glm::vec2> mName2Vector2Map;
//	std::map<int, glm::vec3> mID2Vector3Map;
//	std::map<std::string, glm::vec3> mName2Vector3Map;
//	std::map<int, glm::vec4> mID2Vector4Map;
//	std::map<std::string, glm::vec4> mName2Vector4Map;
//	std::map<int, glm::mat4x4> mID2Mat4x4Map;
//	std::map<std::string, glm::mat4x4> mName2Mat4x4Map;
//};
//
//class Material {
//public:
//	Material() {};
//
//private:
//	//Shader mShader;
//	MaterialPropertyBlock mProperty;
//
//};
//
//
//class CPipeline {
//public:
//	CPipeline() {};
//
//private:
//	friend class VulkanApplication;
//
//};