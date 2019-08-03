#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D sampleTexture;


layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outnormal;
layout(location = 1) out vec4 outcolor;
layout(location = 2) out vec4 outposition;

void main(){
	outcolor = texture(sampleTexture,fragTexCoord.xy); 
	outcolor.a = 1.0;
	outnormal = vec4(normal,0);
	outposition = vec4(worldPos,1);
}