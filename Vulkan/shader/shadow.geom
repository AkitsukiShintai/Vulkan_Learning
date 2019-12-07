#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;

layout (binding = 1) uniform UBO 
{
	mat4 projection[4];
} ubo;
//四级对应↖ ↗ ↙ ↘;

void main(void)
{	

	//左上	
	for(int i=0; i<gl_in.length(); i++)
	{
		vec3 pos = gl_in[i].gl_Position.xyz;
		vec4 pos1 = ubo.projection[0] * (vec4(pos, 1.0));
		pos1 = pos1 / pos1.w;
		pos1.xy = pos1.xy * 0.5 +0.5;
		if(pos1.x <= 1){
			gl_ClipDistance[0] = pos1.x;
		}else{
			gl_ClipDistance[0] = 1.0 - pos1.x;
		}

		if(pos1.y <= 1){
			gl_ClipDistance[1] = pos1.y;
		}else{
			gl_ClipDistance[1] = 1.0 - pos1.y;
		}
		
		pos1.x -= 1.0f;
		//pos1.y = pos1.y*0.5 + 0.5;
		gl_Position = pos1;
		EmitVertex();		
	}
	EndPrimitive();

	//右上	
	for(int i=0; i<gl_in.length(); i++)
	{
		vec3 pos = gl_in[i].gl_Position.xyz;		
		vec4 pos1 = ubo.projection[1] * (vec4(pos, 1.0));
		pos1 = pos1 / pos1.w;
		pos1.xy = pos1.xy * 0.5 +0.5;
		if(pos1.x <= 1){
			gl_ClipDistance[0] = pos1.x;
		}else{
			gl_ClipDistance[0] = 1.0 - pos1.x;
		}

		if(pos1.y <= 1){
			gl_ClipDistance[1] = pos1.y;
		}else{
			gl_ClipDistance[1] = 1.0 - pos1.y;
		}
		gl_Position = pos1;
		EmitVertex();	
	}
	EndPrimitive();

	//左下	
	for(int i=0; i<gl_in.length(); i++)
	{
		vec3 pos = gl_in[i].gl_Position.xyz;		
		vec4 pos1 = ubo.projection[2] * (vec4(pos, 1.0));
		pos1 = pos1 / pos1.w;
		pos1.xy = pos1.xy * 0.5 +0.5;
		if(pos1.x <= 1){
			gl_ClipDistance[0] = pos1.x;
		}else{
			gl_ClipDistance[0] = 1.0 - pos1.x;
		}

		if(pos1.y <= 1){
			gl_ClipDistance[1] = pos1.y;
		}else{
			gl_ClipDistance[1] = 1.0 - pos1.y;
		}
		pos1.x -=1.0f;
		pos1.y -=1.0f;
		
		gl_Position = pos1;
		EmitVertex();	
	}
	EndPrimitive();
	
	//右下
	for(int i=0; i<gl_in.length(); i++)
	{
		vec3 pos = gl_in[i].gl_Position.xyz;		
		vec4 pos1 = ubo.projection[3] * (vec4(pos, 1.0));
		pos1 = pos1 / pos1.w;
		pos1.xy = pos1.xy * 0.5 +0.5;
		if(pos1.x <= 1){
			gl_ClipDistance[0] = pos1.x;
		}else{
			gl_ClipDistance[0] = 1.0 - pos1.x;
		}

		if(pos1.y <= 1){
			gl_ClipDistance[1] = pos1.y;
		}else{
			gl_ClipDistance[1] = 1.0 - pos1.y;
		}
		pos1.y -=1.0f;
		gl_Position = pos1;
		EmitVertex();	
	}
	EndPrimitive();

	//test	
	// for(int i=0; i<gl_in.length(); i++)
	// {
	// 	vec3 pos = gl_in[i].gl_Position.xyz;
	// 	vec4 pos1 = ubo.projection[0] * (vec4(pos, 1.0));
	// 	pos1 = pos1 / pos1.w;
		
	// 	gl_Position = pos1;
	// 	EmitVertex();		
	// }
	// EndPrimitive();
	
}