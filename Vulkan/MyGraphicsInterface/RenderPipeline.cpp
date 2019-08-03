//#include"VulkanApplication.h"
//#include"RenderPipeline.h"
//#include <fstream>


//
//Shader::Shader(const std::string& shaderPath, const ShaderStage stageBits, const char* mainFunc)
//	:mPath(shaderPath)
//	, mStage(stageBits)
//	, mMainFunc(mainFunc)
//{
//	VulkanApplication* app = static_cast<VulkanApplication*>(Application::GetInstance());
//	auto code = ReadFile(shaderPath);
//	VkShaderModuleCreateInfo createInfo = {};
//	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//	createInfo.codeSize = code.size();
//
//	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
//	if (vkCreateShaderModule(app->GetDevice(), &createInfo, nullptr, &mModule) != VK_SUCCESS) {
//		throw std::exception("failed to create shader module!");
//	}
//}
////
//Shader::Shader(const Shader& src):
//	mPath(src.mPath),
//	mStage(src.mStage),
//	mModule(src.mModule),
//	mMainFunc(src.mMainFunc)
//{
//}
//
//Shader::~Shader() {
//}
//
//void Shader::Destroy() {
//	VulkanApplication* app = static_cast<VulkanApplication*>(Application::GetInstance());
//	vkDestroyShaderModule(app->GetDevice(), mModule, nullptr);
//}
//
//std::string Shader::GetPath() {
//	return mPath;
//}
//
//ShaderStage Shader::GetStage() {
//	
//	return mStage;
//}
//
//
////MaterialPropertyBlock
//MaterialPropertyBlock::MaterialPropertyBlock() {
//
//}
//void MaterialPropertyBlock::SetVector(int nameID, glm::vec2 value) {
//	mID2Vector2Map[nameID] = value;
//}
//void MaterialPropertyBlock::SetVector(std::string bindingName, glm::vec2 value) {
//	mName2Vector2Map[bindingName] = value;
//}
//void MaterialPropertyBlock::SetVector(int nameID, glm::vec3 value) {
//	mID2Vector3Map[nameID] = value;
//}
//void MaterialPropertyBlock::SetVector(std::string bindingName, glm::vec3 value) {
//	mName2Vector3Map[bindingName] = value;
//}
//void MaterialPropertyBlock::SetVector(int nameID, glm::vec4 value) {
//	mID2Vector4Map[nameID] = value;
//}
//void MaterialPropertyBlock::SetVector(std::string bindingName, glm::vec4 value) {
//	mName2Vector4Map[bindingName] = value;
//}
//void MaterialPropertyBlock::SetMatrix(int nameID, glm::mat4x4 value) {
//	mID2Mat4x4Map[nameID] = value;
//}
//void MaterialPropertyBlock::SetMatrix(std::string bindingName, glm::mat4x4 value) {
//	mName2Mat4x4Map[bindingName] = value;
//}
//void MaterialPropertyBlock::SetColor(int nameID, glm::vec4 value) {
//	mID2Vector4Map[nameID] = value;
//}
//void MaterialPropertyBlock::SetColor(std::string bindingName, glm::vec4 value) {
//	mName2Vector4Map[bindingName] = value;
//}
//
//
//
//
//
////Pipeline
//Pipeline::Pipeline(std::vector<Shader> shaders,
//	std::vector<PipelineBindingInfo> bindingInfos,
//	std::vector<PipelineAttributeInfo>attributeInfos,
//	std::vector<PipelineDescriptorSetInfo>descriptors,
//	Topology topology,
//	Rect viewport,
//	IntRect scissor
//) {
//
//	VulkanApplication* app = static_cast<VulkanApplication*>(Application::GetInstance());
//	std::vector<VkPipelineShaderStageCreateInfo> stages;
//	for (size_t i = 0; i < shaders.size(); i++)
//	{
//		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
//		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		shaderStageInfo.stage = static_cast<VkShaderStageFlagBits>(shaders[i].mStage);
//		shaderStageInfo.module = shaders[i].mModule;
//		shaderStageInfo.pName = shaders[i].mMainFunc;
//		stages.push_back(shaderStageInfo);
//	}
//
//	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
//	//binding infos
//	std::vector<VkVertexInputBindingDescription> bindings(bindingInfos.size());
//	for (size_t i = 0; i < bindingInfos.size(); i++)
//	{
//		bindings[i].binding = i;
//		bindings[i].stride = bindingInfos[i].size;
//		bindings[i].inputRate = static_cast<VkVertexInputRate>(bindingInfos[i].inputRate);
//	}
//	//attributes
//	std::vector<VkVertexInputAttributeDescription> attributes(attributeInfos.size());
//	for (size_t i = 0; i < attributeInfos.size(); i++)
//	{
//		attributes[i].binding = attributeInfos[i].binding;
//		attributes[i].location = attributeInfos[i].location;
//		attributes[i].format = static_cast<VkFormat>(attributeInfos[i].format);
//		attributes[i].offset = attributeInfos[i].offset;
//	}
//	vertexInputInfo.vertexBindingDescriptionCount = bindings.size();
//	vertexInputInfo.vertexAttributeDescriptionCount = attributes.size();
//	vertexInputInfo.pVertexBindingDescriptions = bindings.data();
//	vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
//
//	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
//	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//	inputAssembly.topology = static_cast<VkPrimitiveTopology>(topology);
//	inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//	VkExtent2D swipChainExtent = app->GetSwipchainExtent();
//
//
//	VkViewport mviewport = {};
//	mviewport.x = viewport.x0;
//	mviewport.y = viewport.y0;
//	mviewport.width = viewport.x1 - viewport.x0;
//	mviewport.height = viewport.y1 - viewport.y0;
//	mviewport.minDepth = 0.0f;
//	mviewport.maxDepth = 1.0f;
//
//	VkRect2D mscissor = {};
//	mscissor.offset = { scissor.x0,scissor.y0 };
//	mscissor.extent = VkExtent2D{static_cast<uint32_t>( scissor.x1 - scissor.x0),static_cast<uint32_t>(scissor.y1 - scissor.y0)};
//
//	VkPipelineViewportStateCreateInfo viewportState = {};
//	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//	viewportState.viewportCount = 1;
//	viewportState.pViewports = &mviewport;
//	viewportState.scissorCount = 1;
//	viewportState.pScissors = &mscissor;
//
//	VkPipelineRasterizationStateCreateInfo rasterizer = {};
//	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//	rasterizer.depthClampEnable = VK_FALSE;
//	rasterizer.rasterizerDiscardEnable = VK_FALSE;
//	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//	rasterizer.lineWidth = 1.0f;
//
//	rasterizer.depthBiasEnable = VK_FALSE;
//	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
//	rasterizer.depthBiasClamp = 0.0f; // Optional
//	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
//	rasterizer.cullMode = VK_CULL_MODE_NONE;
//	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//
//	VkPipelineMultisampleStateCreateInfo multisampling = {};
//	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//	multisampling.sampleShadingEnable = VK_FALSE;
//	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//	multisampling.minSampleShading = 1.0f; // Optional
//	multisampling.pSampleMask = nullptr; // Optional
//	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
//	multisampling.alphaToOneEnable = VK_FALSE; // Optional
//
//	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
//	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachment.blendEnable = VK_FALSE;
//	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
//	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
//
//	VkPipelineColorBlendStateCreateInfo colorBlending = {};
//	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//	colorBlending.logicOpEnable = VK_FALSE;
//	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
//	colorBlending.attachmentCount = 1;
//	colorBlending.pAttachments = &colorBlendAttachment;
//	colorBlending.blendConstants[0] = 1.0f; // Optional
//	colorBlending.blendConstants[1] = 0.0f; // Optional
//	colorBlending.blendConstants[2] = 0.0f; // Optional
//	colorBlending.blendConstants[3] = 0.0f; // Optional
//
//	VkDynamicState dynamicStates[] = {
//		VK_DYNAMIC_STATE_VIEWPORT,
//		VK_DYNAMIC_STATE_SCISSOR,
//	};
//
//	VkPipelineDynamicStateCreateInfo dynamicState = {};
//	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	dynamicState.dynamicStateCount = 2;
//	dynamicState.pDynamicStates = dynamicStates;
//
//	std::vector<VkDescriptorSetLayout> laytous(1);
//	std::vector<VkDescriptorSetLayoutBinding> layoutBindings(descriptors.size());
//	for (int i = 0; i< descriptors.size();++i)
//	{
//		layoutBindings[i].binding = descriptors[i].binding;
//		layoutBindings[i].descriptorCount = descriptors[i].count;
//		layoutBindings[i].descriptorType = static_cast<VkDescriptorType> (descriptors[i].type);
//		layoutBindings[i].pImmutableSamplers = nullptr;
//		layoutBindings[i].stageFlags = static_cast<VkShaderStageFlags> (descriptors[i].stageFlags);;
//	}
//	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//	layoutInfo.bindingCount = layoutBindings.size();
//	layoutInfo.pBindings = layoutBindings.data();
//	if (vkCreateDescriptorSetLayout(app->GetDevice(), &layoutInfo, nullptr, laytous.data()) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create descriptor set layout!");
//	}
//
//	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutInfo.setLayoutCount = laytous.size(); // Optional
//	pipelineLayoutInfo.pSetLayouts = laytous.data(); // Optional
//	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
//	pipelineLayoutInfo.pPushConstantRanges = 0; // Optional
//
//	if (vkCreatePipelineLayout(app->GetDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
//		throw std::exception("failed to create pipeline layout!");
//	}
//
//
//
//	VkGraphicsPipelineCreateInfo pipelineInfo = {};
//	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//	pipelineInfo.stageCount = stages.size();
//	pipelineInfo.pStages = stages.data();
//	pipelineInfo.pVertexInputState = &vertexInputInfo;
//	pipelineInfo.pInputAssemblyState = &inputAssembly;
//	pipelineInfo.pViewportState = &viewportState;
//	pipelineInfo.pRasterizationState = &rasterizer;
//	pipelineInfo.pMultisampleState = &multisampling;
//	pipelineInfo.pDepthStencilState = nullptr; // Optional
//	pipelineInfo.pColorBlendState = &colorBlending;
//	pipelineInfo.pDynamicState = &dynamicState; // Optional
//	pipelineInfo.layout = mPipelineLayout;
//	pipelineInfo.renderPass = app->GetRenderPass();
//	pipelineInfo.subpass = 0;
//	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//	pipelineInfo.basePipelineIndex = -1; // Optional
//	VkResult r = vkCreateGraphicsPipelines(app->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline);
//	if (r != VK_SUCCESS) {
//		throw std::exception("failed to create graphics pipeline!");
//	}
//
//}
