#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D normalTex;
layout(binding = 1) uniform sampler2D colorTex;
layout(binding = 2) uniform sampler2D posTex;
layout(binding = 3) uniform sampler2D depthBuffer;

layout(binding = 4) uniform LightInfo{  
    vec3 lightPos;
    float  ambientStrenth;
	vec3 lightColor;
}light;
layout(binding = 5) uniform CameraInfor{  
    mat4 view;
    mat4 proj;
    vec3 pos;
}ubo;

layout(binding = 6) uniform sampler2D shadowDepth;
layout(binding = 7) uniform ShadowMatrixs {

mat4 projs[4];

}shadowMats;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 color;

vec2 CalculateSSR(vec3 worldPos,vec3 reflectDir, vec3 normal){
	float dirLength = 0.1;
	float dis =0.1;
	vec2 uv;
	while( dirLength < 10.0f){
		
		vec3 nextPos = worldPos + reflectDir * dirLength;
		vec4 projPos = ubo.proj *  ubo.view *vec4(nextPos,1.0);
		vec4 depthPos = projPos / projPos.w;
		uv = depthPos.xy * 0.5 + 0.5;
		float sampleDepth = texture(depthBuffer,uv).x;
		if(depthPos.z > sampleDepth && depthPos.z - sampleDepth < 0.0001){	
			return uv;
		}
		//dis *= 2.0;
		dirLength += dis;		
	}	
	
	return vec2(-1.0);
}
const float MAX_DIS = 20.0f;
const float STEP = 0.1f;
vec2 CalculateSSR1(vec2 screenUV){
	vec4 worldpos = texture(posTex,screenUV);
	vec4 view = ubo.view * worldpos;
	vec4 normal = texture(normalTex,screenUV);
	normal = ubo.view * normal;

	vec3 reflectDir = reflect(normalize(view.xyz),normalize(normal.xyz));

	float dirLength = STEP;
	float dis =STEP;
	vec2 uv;
	vec3 hitPos;
	while( dirLength < MAX_DIS){		
		hitPos = view.xyz + reflectDir * dirLength;
		vec4 projPos = ubo.proj *vec4(hitPos,1.0);
		vec4 depthPos = projPos / projPos.w;
		uv = depthPos.xy * 0.5 + 0.5;
		float sampleDepth = texture(depthBuffer,uv).x;
		if(depthPos.z > sampleDepth){	
			vec3 pos = (ubo.view * texture(posTex,uv)).xyz;
			if(length(pos - hitPos) > 0.1){
				dirLength += dis;	
				continue;
			}
			float step = 1;
			int time = 10;
			//bool isOut = false;
			while(time >=0){
				 step *= 0.5;
				if(sampleDepth < depthPos.z)
                {
                    hitPos -= reflectDir * step;
                }
                else{
                    hitPos += reflectDir * step;
                }
				projPos = ubo.proj  * vec4(hitPos,1.0);
				depthPos = projPos / projPos.w;
				uv = depthPos.xy * 0.5 + 0.5;
                sampleDepth = texture(depthBuffer, uv).x;			
                if(abs(sampleDepth - depthPos.z) < 0.000001)
                {
                    return uv;
                }
				time -- ;
			}
			return vec2(-1.0);
		}
		//dis *= 2.0;
		dirLength += dis;		
	}	
	
	return vec2(-1.0);
}

// vec2 StartSSR(in vec3 dir, in vec3 pos)
// {
//     vec3 vsnormal = normalize(texture(normalTex, inFillRectPos * 0.5 + 0.5).xyz * 2.0 - 1.0);
//     vec3 ray = normalize(reflect(dir, vsnormal));
//     float acc_leng = 0.0;
//     while(acc_leng < MAX_LENGTH)
//     {
//         acc_leng += TRACE_STEP;
//         vec3 trace_pos = pos + ray * acc_leng;
//         vec4 clip_space = CameraData.mProjectionMat * vec4(trace_pos, 1.0);
//         vec3 depthpos = clip_space.xyz / clip_space.w;
//         vec2 uv = depthpos.xy * 0.5 + 0.5;
//         float ref = texture(depthTex, uv).x;
//         if(ref < depthpos.z)
//         {
//             float step = TRACE_STEP;
//             for(int i = 0; i < REFINE_TIMES; ++i)
//             {
//                 step *= 0.5;
//                 if(ref < depthpos.z)
//                 {
//                     trace_pos -= ray * step;
//                 }
//                 else{
//                     trace_pos += ray * step;
//                 }
//                 clip_space = CameraData.mProjectionMat * vec4(trace_pos, 1.0);
//                 depthpos = clip_space.xyz / clip_space.w;
//                 uv = depthpos.xy * 0.5 + 0.5;
//                 ref = texture(depthTex, uv).x;
//                 if(abs(ref - depthpos.z) < EPSILON)
//                 {
//                     return uv;
//                 }
//             }
//             return vec2(-1.0);
//         }
//     }
//     return vec2(-1.0);
// }
const float raystpe = 1;
const float minRayStep =2;
const int maxStep = 20;
const float searchDis = 5;
const int binarySearchMaxStep = 10;
const vec3 CASCADED_LEVEL = vec3(0.067f,0.067f + 0.133f , 0.067f + 0.133f + 0.267f);
vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth){
	float depth;
	vec4 projPos;
	for(int i = 0; i < binarySearchMaxStep; i++)
    {

        projPos = ubo.proj * ubo.view*vec4(hitCoord, 1.0);
        projPos.xyz /= projPos.w;
        projPos.xy = projPos.xy * 0.5 + 0.5;
 
       depth = (texture(depthBuffer, projPos.xy)).x;

 
        dDepth = projPos.z - depth;
        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord -= dir;
        else
            hitCoord += dir;    
    }

	if(abs(dDepth)> 0.0001){
		projPos = vec4(-1.0);
	}
    return vec3(projPos.xy, depth);


}


vec4 Raycast(vec3 dir, inout vec3 hitCoord, out float dDepth){
	if(normalize(vec4(ubo.view * vec4(dir,0))).z > 0.5f){
		return vec4(-2,-1,0,1.0);
	}
	 dir *= raystpe;
	 float depth= 0.0;
	 int steps = 0;
	 vec4 projPos = vec4(-1.0);
	for(int i = 0; i< maxStep ; i++){
		hitCoord += dir;
		projPos = ubo.proj *ubo.view* vec4(hitCoord,1.0);
		projPos /= projPos.w;
		projPos.xy = projPos.xy * 0.5 + 0.5;
		
		depth = (texture(depthBuffer,projPos.xy)).x;
		if(depth == 1||depth == 0)
			continue;

		dDepth = projPos.z - depth;
		if(dDepth < 0.00001){
			
			if(dDepth >=0.0){			
				vec4 result;
				result = vec4(binarySearch(dir,hitCoord,dDepth),1.0);
				return result;					
			}
		}
		
		steps ++;
	}
	projPos = vec4(-1.0);
	return vec4(projPos.xy,depth,1.0);
}

vec2 nearFarPlane = vec2(0.1,1000.0);
vec4 _LightSplitsNear = vec4(0,0.067f,0.067f + 0.133f , 0.067f + 0.133f + 0.267f)* (nearFarPlane.y - nearFarPlane.x) + nearFarPlane.x;
vec4 _LightSplitsFar= vec4(0.067f,0.067f + 0.133f , 0.067f + 0.133f + 0.267f,1.0)*  (nearFarPlane.y - nearFarPlane.x) + nearFarPlane.x;
vec4 getCascadeWeights(float z)
{
    vec4 zNear = vec4( z >= _LightSplitsNear.x, z >= _LightSplitsNear.y,z >= _LightSplitsNear.z,z >= _LightSplitsNear.w) ;
    vec4 zFar = vec4( z < _LightSplitsFar.x,z < _LightSplitsFar.y,z < _LightSplitsFar.z,z < _LightSplitsFar.w ) ;
    vec4 weights = zNear * zFar;

    return weights;
}
//
float GetShadow(vec4 worldPos,vec4 cascadeWeights){
	vec4 projPos0 = shadowMats.projs[0] * worldPos;
	vec4 projPos1 = shadowMats.projs[1] * worldPos;
	vec4 projPos2 = shadowMats.projs[2] * worldPos;
	vec4 projPos3 = shadowMats.projs[3] * worldPos;
	
	projPos0 /= projPos0.w;
	projPos1 /= projPos1.w;
	projPos2 /= projPos2.w;
	projPos3 /= projPos3.w;

	//scale to (0,0) to (1,1)
	projPos0.xy = projPos0.xy *0.5 +0.5;
	projPos1.xy = projPos1.xy *0.5 +0.5;
	projPos2.xy = projPos2.xy *0.5 +0.5;
	projPos3.xy = projPos3.xy *0.5 +0.5;

	//move to corner,the coord become (-1,-1) to (1,1)
	projPos0.x -= 1.0;
	projPos2.xy -= 1.0;
	projPos3.y -= 1.0;
	float sampleDepth0 ;
	float sampleDepth1 ;
	float sampleDepth2 ;
	float sampleDepth3 ;
	
	float depth0 = projPos0.z;
    float depth1 = projPos1.z;
    float depth2 = projPos2.z;
    float depth3 = projPos3.z;
	float _gShadowStrength = 1.0f;
	//比较精确的bias
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0.005f;


	float shadow0 ;
	float shadow1 ;
	float shadow2 ;
	float shadow3 ;
	vec2 texelSize = 1.0 / textureSize(shadowDepth, 0);
	if(projPos0.x >= -1.0 && projPos0.x <= 0 && projPos0.y <= 1.0 &&projPos0.y >= 0){
		projPos0.xy = projPos0.xy *0.5 +0.5;
		//sampleDepth0 = texture(shadowDepth,projPos0.xy).x;
		
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				sampleDepth0 = texture(shadowDepth, projPos0.xy + vec2(x, y) * texelSize).r; 
				shadow0 += depth0 - bias > sampleDepth0 ? 1.0 : 0.0;        
			}    
		}
		shadow0  /= 9.0;



	}else{
		sampleDepth0 = 1.0;
	}

	
	if(projPos1.x >= 0.0 && projPos1.x <= 1.0 && projPos1.y <= 1.0 &&projPos1.y >= 0){
		projPos1.xy = projPos1.xy *0.5 +0.5;
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				sampleDepth1 = texture(shadowDepth, projPos1.xy + vec2(x, y) * texelSize).r; 
				shadow1 += depth1 - bias > sampleDepth1 ? 1.0 : 0.0;        
			}    
		}
		shadow1  /= 9.0;

	}else{
		sampleDepth1 = 1.0;
	}


	if(projPos2.x >= -1.0 && projPos2.x <= 0 && projPos2.y <= 0.0 &&projPos2.y >= -1.0){
		projPos2.xy = projPos2.xy *0.5 +0.5;
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				sampleDepth2 = texture(shadowDepth, projPos2.xy + vec2(x, y) * texelSize).r; 
				shadow2 += depth2 - bias > sampleDepth2 ? 1.0 : 0.0;        
			}    
		}
		shadow2  /= 9.0;
	}else{
		sampleDepth2 = 1.1;
	}

	if(projPos3.x >= 0.0 && projPos3.x <= 1.0 && projPos3.y <= 0.0 &&projPos3.y >= -1.0){
		projPos3.xy = projPos3.xy *0.5 +0.5;
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				sampleDepth3 = texture(shadowDepth, projPos3.xy + vec2(x, y) * texelSize).r; 
				shadow3 += depth3 - bias > sampleDepth3 ? 1.0 : 0.0;        
			}    
		}
		shadow3  /= 9.0;
	}else{
		sampleDepth3 = 1.1;
	}

	
	// float shadow0 = (depth0 - sampleDepth0)>bias ? _gShadowStrength : 0;
    // float shadow1 = (depth0 - sampleDepth0)>bias ? _gShadowStrength : 0;
    // float shadow2 = (depth0 - sampleDepth0)>bias ? _gShadowStrength : 0;
    // float shadow3 = (depth0 - sampleDepth0)>bias ? _gShadowStrength : 0;
	

	float shadow = shadow0*cascadeWeights[0] + shadow1*cascadeWeights[1] + shadow2*cascadeWeights[2] + shadow3*cascadeWeights[3];
	return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowDepth, projCoords.xy).x; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float shadow = (currentDepth - closestDepth)>0.0001  ? 1.0 : 0.0;

    return shadow;
}

void main(){
	vec4 diffusecolor = texture(colorTex,fragTexCoord.xy); 

	vec4 normal =  texture(normalTex,fragTexCoord.xy); 
	vec4 worldPos = texture(posTex,fragTexCoord.xy);
	vec4 projPos =  ubo.view * worldPos;
	//projPos/= projPos.w;
	vec4 weight = getCascadeWeights(-projPos.z);
	color = worldPos;
	return;
	//float shadow = GetDepth(worldPos,weight);
	//vec4 shadowProj = shadowMats.projs[0] * worldPos;
	//shadowProj /= shadowProj.w;
	float shadow = GetShadow(worldPos,weight);
	// vec4 clicp = vec4(fragTexCoord.xy*2 - 1,texture(depthBuffer,fragTexCoord).x,1.0);
	// vec4 view = inverse(ubo.proj)*clicp;
	// view /= view.w;
	// worldPos = (inverse(ubo.view)*view).xyz;
	//normal = (ubo.view * vec4(normal,0.0)).xyz ;
	//vec3 viewPos = worldPos;//(ubo.view * vec4(worldPos,1.0)).xyz;
	//outcolor = vec4(color,1.0);
	//return;

	//vec3 r = normalize((ubo.view *vec4(normalize(reflect(-normalize(worldPos - ubo.pos),normalize(normal))),0.0)).xyz);
	//vec3 r = normalize(reflect(normalize(worldPos - ubo.pos),normalize(normal))) ;

	//vec2 reflectColorUV = CalculateSSR(worldPos,r,normal);
	//color = vec4(normalize(texture(normalTex,fragTexCoord).xyz) * 0.5 +0.5, 1.0);
	//color = vec4(fragTexCoord,0.0,1.0);
	//float dDepth;
	//vec2 reflectColorUV = CalculateSSR(worldPos,r,normal);
	//vec2 reflectColorUV = CalculateSSR1(fragTexCoord);

	//vec4 reflection = Raycast(r,worldPos,dDepth);
	//vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - reflection.xy));
	//float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	// if(reflection.x < 0){
	// 	if(reflection.x == -2){
	// 		color = vec4(1,0,0,1);
	// 	}else{
	// 		color = vec4(0,1,0,1);
	// 	}
	// 	//color = vec4(fragTexCoord,0,1);
	// }
	// else{
	// 	color = texture(colorTex,reflection.xy);
	// 	//color = vec4(reflection.xy,0,1);
	// }
	
	// if(reflectColorUV.x > 0){

	// 	color = texture(colorTex,reflectColorUV);

	// }else{

	// 	color = texture(colorTex,fragTexCoord);

	// }
	
	color =  0.1f + (1.0 - shadow) * diffusecolor;  

	//color = vec4(diffusecolor.xyz,1.0);
	//color = vec4(worldPos,1.0f);
	//color = (1-texture(depthBuffer,fragTexCoord).x) * vec4(1.0);
	//vec3 I = normalize(worldPos - cameraPos);
    //vec3 R = reflect(I, normalize(Normal));
	//color = texture(skybox, R);
	//color = 
}