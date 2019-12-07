#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform CameraInfor{  
    mat4 view;
    mat4 proj;
    vec3 pos;
}ubo;

layout(binding = 5) uniform ModMat{  
    mat4 modling;
}modlingMat;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTangent;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTexCoord1;
layout(location = 5) in vec4 inTexCoord2;

layout(location = 0) out vec3 worldPos;

void main() {
    //modling = transpose(modling);
    vec4 pos = ubo.proj * ubo.view * modlingMat.modling * vec4(inPosition,1.0);  
    gl_Position =  pos; 
    worldPos =(modlingMat.modling * vec4(inPosition,1.00)).xyz;  
    // vec2 uv = pos * 0.1;
    // vec4 cameraRay = vec4(uv,1.0,1.0);
    // mat4 inverseCameraPro = inverse(ubo.proj);
    // cameraRay = inverseCameraPro * cameraRay;   
    // ray = cameraRay.xyz / cameraRay.w; 
}