#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D normalTex;
layout(binding = 1) uniform sampler2D colorTex;
layout(binding = 2) uniform sampler2D position;
layout(binding = 3) uniform LightInfo{  
    vec3 lightPos;
    float  ambientStrenth;
	vec3 lightColor;
}light;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 color;

void main(){
	color = texture(colorTex,fragTexCoord.xy); 
	//vec3 I = normalize(worldPos - cameraPos);
    //vec3 R = reflect(I, normalize(Normal));
	//color = texture(skybox, R);
	//color = 
}