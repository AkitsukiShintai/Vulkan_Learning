#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D textureSamp;


layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 worldPos;
layout(location = 2) in vec3 fragnormal;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 color;


void main(){
    color = texture(textureSamp,fragTexCoord).xyz;
    position = worldPos;
    normal = normalize(fragnormal);
}