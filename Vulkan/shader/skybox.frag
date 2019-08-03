#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform samplerCube cubeMap;

layout(location = 0) in vec3 fragTexCoord;

layout(location = 0) out vec4 skyColor;

void main(){

	// float x = skyColor.x;
	// skyColor.x = skyColor.z;
	// skyColor.z = x;
	//skyColor.y = x;
	//skyColor = texture(textureSamp,fragTexCoord.xy); 
	skyColor = texture(cubeMap,fragTexCoord); 
	//skyColor = vec4(1,1,0,1);
	//skyColor = vec4(1,0,0,1);
}