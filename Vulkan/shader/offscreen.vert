#version 450
#extension GL_ARB_separate_shader_objects : enable



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTangent;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec3 inTexCoord1;
layout(location = 5) in vec4 inTexCoord2;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    //modling = transpose(modling);
    vec2 pos = inPosition.xz;
    gl_Position = vec4(pos * 0.1,0.0,1.0) ;  
    fragTexCoord.xy =inTexCoord0;
}