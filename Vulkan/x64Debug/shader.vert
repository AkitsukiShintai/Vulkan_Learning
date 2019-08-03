#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTexCoord2;
layout(location = 4) in vec4 inTexCoord3;




layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 worldPos;
layout(location = 2) out vec3 normal;
void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    worldPos = (ubo.model *vec4(inPosition, 1.0)).xyz;
    normal =(inverse(transpose(ubo.model)) * vec4(inNormal,0.0f)).xyz;
    fragTexCoord = inTexCoord;
}