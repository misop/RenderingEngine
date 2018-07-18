#version 430

#define POINT_LIGHTS 8

layout(location = 0) in vec4 vertex_eye;
layout(location = 1) in vec4 normal_eye;
layout(location = 2) in vec4 light_eye;
layout(location = 5) in vec2 uv;
layout(location = 6) in vec4 shadowCoord;
layout(location = 7) in vec4 point_eye[POINT_LIGHTS];

layout(location = 1) uniform mat4 ViewMatrix;
layout(location = 4) uniform vec4 Material;
layout(location = 11) uniform vec4 SunColor;
layout(location = 20) uniform vec4 PointColor[POINT_LIGHTS];
layout(location = 28) uniform vec4 Attenuation[POINT_LIGHTS];
layout(binding=0) uniform sampler2D DiffuseSampler;
layout(binding=3) uniform samplerCube cube_map;
//layout(binding=4) uniform sampler2DShadow ShadowSampler;

const float zNear = 1.0;
const float zFar = 15000;

// aluminium
//const float d = 10.2958;
//const float ps = 1.3547;
//const float alpha = 0.01;
// chrome
//const float d = 8.8524;
//const float ps = 0.8714;
//const float alpha = 0.0091;
// polyethylen
//const float d = 3.673;
//const float ps = 0.069;
//const float alpha = 0.0423;
// transparency
const float d = 0.9388;
const float ps = 0.2190;
const float alpha = 0.0033;
// paper
//const float d = 0.9989;
//const float ps = 0.0016;
//const float alpha = 0.6896;

const float exposure = 4;

layout (location = 0) out vec4 fColor;

void main(void)
{	   
	vec4 diffuse_material = texture(DiffuseSampler, uv);
	vec4 color = vec4(0, 0, 0, 1);

	vec4 V = normalize(vertex_eye);
   	vec4 L = normalize(light_eye);
   	vec4 N = normalize(normal_eye);
   	vec4 H = normalize(L + V); 
	vec4 R = normalize(reflect(V, N));
	
	R = normalize(inverse(ViewMatrix)*R);	
	vec4 light = texture(cube_map, vec3(R.x, -R.y, R.z));
	fColor = light;
	//fColor = vec4(1, 0.41, 0.71, 1);
}