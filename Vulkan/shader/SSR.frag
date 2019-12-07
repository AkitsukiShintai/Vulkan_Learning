#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(binding = 0) uniform CameraInfor{  
    mat4 view;
    mat4 proj;
    vec3 pos;
}ubo;

layout(binding = 1) uniform sampler2D normalTex;
layout(binding = 2) uniform sampler2D colorTex;
layout(binding = 3) uniform sampler2D worldTex;
layout(binding = 4) uniform sampler2D depthTex;

layout(location = 0) in vec3 worldPos;

layout(location = 0) out vec4 outcolor;

void main(){
	//world
	vec4 projPos = ubo.proj * ubo.view * vec4(worldPos,1.0);
	vec2 fragTexCoord = (projPos / projPos.w).xy * 0.5 + 0.5;
	vec3 normal = texture(normalTex,fragTexCoord.xy).xyz; 
	vec3 color = texture(colorTex,fragTexCoord.xy).xyz; 
	vec3 worldPos = texture(worldTex,fragTexCoord.xy).xyz;
	normal = (ubo.view * vec4(normal,0.0)).xyz ;
	vec3 viewPos = (ubo.view * vec4(worldPos,1.0)).xyz;
	//outcolor = vec4(color,1.0);
	//return;
	vec3 r = normalize(reflect(normalize(viewPos),normal)) ;

	float deltaDepth = -1.0;
	int time = 10;
	float dirLength = 1.0;
	vec2 uv;
	while( deltaDepth < 0.0 || time >=0){
		vec3 nextPos = viewPos + r * dirLength;
		vec4 projPos = ubo.proj * vec4(nextPos,1.0);
		vec4 depthPos = projPos / projPos.w;
		uv = depthPos.xy * 0.5 + 0.5;
		float sampleDepth = texture(depthTex,uv).x;
		deltaDepth = depthPos.z - sampleDepth;
		dirLength += (2 * dirLength);
		time--;
	}
	if(deltaDepth > 0.0){
		outcolor =  texture(colorTex,uv);
	}else{

		outcolor =  vec4(0,0,0,0);
	}
}