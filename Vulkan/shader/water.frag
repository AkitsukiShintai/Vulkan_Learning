#version 450
#extension GL_ARB_separate_shader_objects : enable




vec3 GetNormalFromMap(sampler2D normalMap,vec2 uv){
    vec3 normal;
    normal.xz = texture(normalMap,uv).xy *2 - 1;
    normal.y = sqrt(clamp(1-normal.x * normal.x - normal.z*normal.z,0.0,1.0)) + 8;
    return normal;
}

vec3 SampleWaterNormal(sampler2D bumpMap,vec2 uvBase, vec2 uvShift){
    vec3 microBumpNormal = vec3(0);
    microBumpNormal += GetNormalFromMap(bumpMap,0.75 * uvBase + 0.4 * uvShift).xyz;
    microBumpNormal += GetNormalFromMap(bumpMap,1 * uvBase + 0.4 * uvShift).xyz;
    microBumpNormal += GetNormalFromMap(bumpMap,1.2 * uvBase + 0.5 * vec2(-uvShift.x,uvShift.y)).xyz*0.6;
    microBumpNormal += GetNormalFromMap(bumpMap,1.5 * uvBase + 0.6 * vec2(uvShift.x,uvShift.y)).xyz*0.8;
    return normalize(microBumpNormal);
}

vec4 SampleWaterColor(sampler2D waterTexture,vec2 uvBase, vec2 uvShift){
    vec4 color;
    color += texture(waterTexture,0.8 * uvBase + 0.4 * uvShift);
    color += texture(waterTexture,1 * uvBase + 0.8 * uvShift);
    color /= 2;
    return color;
}

vec2 GetWaterTexOffset(float speed,float totalTime,vec2 waterDir){

    return totalTime * speed * 0.025 * normalize(waterDir);

}



layout(binding = 0) uniform sampler2D waterTexture;
layout(binding = 1) uniform sampler2D waterNormalTexture;
layout(binding = 2) uniform sampler2D depthTexture;

layout(binding = 3) uniform  WaterParameter
{
    vec4 waterBaseColor;
    vec2 waterDir;
    float fresnel;
    float refractionBumpScale;
    float specPower;
    float waterSpecularIntensity;
    float waterVisibility;
    float waterVisibilityPower;
    float waterRoughness;
    float sampleLength;
    float waterSpeed;
    float waterHeight;
    float totalTime;
    float waveHeight;
}water;
layout(binding = 4) uniform  Modeling{
    mat4 modelingMat; 

}ObjToWorldMat;

layout(binding = 5) uniform LightInfo{
    vec3 lightPos;
	float  ambientStrenth;
	vec3 lightColor;
	vec3 lightDir;
	int type;
}lightInfo;


layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 worldPos;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 normal;

layout(location = 0) out vec4 color;


void main(){
	vec2 uv = fragTexCoord;
    vec3 sampleNormal;
    vec2 texOffset = GetWaterTexOffset(water.waterSpeed,water.totalTime,water.waterDir);
    sampleNormal = SampleWaterNormal(waterNormalTexture,uv,texOffset);
    sampleNormal.y /= max(0.01,water.waveHeight);
    vec3 normalWorld = normalize((ObjToWorldMat.modelingMat * vec4(normal,0.0)).xyz);
    //normal tangent space to world space
    vec3 tangentWorld = normalize((ObjToWorldMat.modelingMat * vec4(tangent,0.0)).xyz );
    vec3 bitangentWorld = normalize(cross(tangentWorld,normalWorld));
    mat3 tangentSpaceToWorld = mat3(tangentWorld,bitangentWorld,normalWorld);

    //translate sampleNormal to world space
    sampleNormal = normalize(tangentSpaceToWorld * sampleNormal);

    vec4 waterColor = SampleWaterColor(waterTexture,uv,texOffset);
     vec3 inLight;
     //dir light
    if(lightInfo.type == 0){
        inLight = -lightInfo.lightPos;
    }else if(lightInfo.type == 1){//point light
        inLight = normalize(lightInfo.lightPos - worldPos);
    }
    float diffuseFactor = max(0.0,dot(sampleNormal,inLight));
    color = waterColor;
}