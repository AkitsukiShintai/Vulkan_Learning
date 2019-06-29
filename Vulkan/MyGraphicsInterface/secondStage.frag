#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform  LightInfo{
    vec3 lightPos;
    float radius;
    vec3 lightColor;
}lightInfo;

layout(binding = 1) uniform sampler2D posSampler;
layout(binding = 2) uniform sampler2D normalSampler;
layout(binding = 3) uniform sampler2D colorSampler;

layout(location = 0) in vec2 outUV;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 worldPos = texture(posSampler, outUV).xyz;
    vec3 normal = texture(normalSampler, outUV).xyz;
    vec3 objectColor = texture(colorSampler, outUV).xyz;
    vec3 nor = normalize(normal);
   
    vec3 lightDir = normalize(lightInfo.lightPos - worldPos);
    
    float diff = max(dot(nor, lightDir), 0.0);
    vec3 diffuse = diff * lightInfo.lightColor;
    vec3 ambient = lightInfo.radius * lightInfo.lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    outColor = vec4(result, 1.0f);
}