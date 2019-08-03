#version 450
#extension GL_ARB_separate_shader_objects : enable




layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 worldPos;
layout(location = 2) in vec3 normal;

layout(binding = 1) uniform sampler2D texSampler;

layout(std140, binding = 2) uniform LightInfo {
    vec3 lightPos;
    vec4 lightColor;
    float ambientStrength;
}lightInfo;
layout(location = 0) out vec4 outColor;


void main() {
    //vec3 objectColor = vec4(lightInfo.lightColor,1.0f) ;
    vec3 objectColor = texture(texSampler, fragTexCoord).xyz;
    outColor = texture(texSampler, fragTexCoord);
    vec3 nor = normalize(normal);
   
    vec3 lightDir = normalize(lightInfo.lightPos - worldPos);
    
    float diff = max(dot(nor, lightDir), 0.0);
    vec3 diffuse = diff * lightInfo.lightColor.xyz;
    vec3 ambient = lightInfo.ambientStrength * lightInfo.lightColor.xyz;
        outColor = vec4(lightInfo.ambientStrength,lightInfo.ambientStrength,lightInfo.ambientStrength,1.0f);
    return;
    vec3 result = (ambient + diffuse) * objectColor;
    outColor = vec4(result, 1.0f);
}