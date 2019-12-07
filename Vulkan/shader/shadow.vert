#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ModelingMatrix{
    mat4 modeling;
}ObjToWorld;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTangent;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTexCoord1;
layout(location = 5) in vec4 inTexCoord2;

void main() {
    //modling = transpose(modling);
   gl_Position = ObjToWorld.modeling * vec4(inPosition,1.0);
}