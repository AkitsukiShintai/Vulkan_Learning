#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform CameraInfor{  
    mat4 view;
    mat4 proj;
    vec3 pos;
}ubo;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTangent;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTexCoord1;
layout(location = 5) in vec4 inTexCoord2;

layout(location = 0) out vec3 fragTexCoord;
void main() {
    //modling = transpose(modling);
    gl_Position = ubo.proj * vec4((ubo.view  * vec4(inPosition, 0.0)).xyz, 1.0);   
    fragTexCoord = inPosition;
}