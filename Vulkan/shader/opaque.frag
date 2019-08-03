#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D sampleTexture;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 cameraPos;
layout(location = 2) in vec3 worldPos;


layout(location = 0) out vec4 color;

void main(){
	color = vec4(1,0,0,0);
	return;
	color = texture(sampleTexture,fragTexCoord.xy); 
	//vec3 I = normalize(worldPos - cameraPos);
    //vec3 R = reflect(I, normalize(Normal));
	//color = texture(skybox, R);
	//color = 
}