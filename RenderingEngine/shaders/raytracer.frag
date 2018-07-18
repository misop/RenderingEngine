#version 430

layout(location = 0) in vec4 vertex_eye;
layout(location = 1) in vec4 normal_eye;
layout(location = 2) in vec4 light_eye;
layout(location = 3) in vec3 tangent_eye;
layout(location = 4) in vec3 bitangent_eye;

layout(location = 1) uniform mat4 ViewMatrix;
layout(location = 4) uniform vec4 Material;
layout(location = 5) uniform vec3 RayDirection;
layout(binding=3) uniform samplerCube cube_map;

// aluminium
//const float d = 0.5;//10.2958;
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
//const float d = 0.9388;
//const float ps = 0.2190;
//const float alpha = 0.0033;
// paper
//const float d = 0.9989;
//const float ps = 0.0016;
//const float alpha = 0.6896;
//custom
const float d = 0;
const float ps = 1;
const float alpha = 0.001;

layout (location = 0) out vec4 fColor;

void main(void)
{	   
	vec4 color = vec4(0, 0, 0, 1);
	vec3 ray_dir = normalize(RayDirection);

	vec4 V = normalize(vertex_eye);
   	vec4 L = normalize(light_eye);
   	vec4 N = normalize(normal_eye);
	vec4 H = normalize(L + V); 
   	vec3 T = normalize(tangent_eye); 
   	vec3 B = normalize(bitangent_eye); 
	vec4 R = normalize(reflect(V, N));
	
	//mat3 TBNi = mat3(T, B, vec3(N));
	//vec4 L = vec4(TBNi * ray_dir, 0.0);
   	//vec4 H = normalize(L + V); 
	
	vec3 E = normalize(-RayDirection);	
	vec4 light = texture(cube_map, E);
	
	float NdotL = clamp(dot(N, L), 0.0, 1.0);
	float NdotV = clamp(dot(N, V), 0.0, 1.0);
	
	if (NdotL > 0 && NdotV > 0) {
		float alpha2 = alpha*alpha;
		float angle = acos(dot(N,H));
		
		vec4 diffuse = NdotL*d*light;
		vec4 specular = light*NdotL*(ps*exp(-pow(tan(angle),2)/alpha2))/(sqrt(max(NdotL*NdotV,0.0000001))*4*3.14159265359*alpha2);
		
		color = diffuse + specular;
		color.a = 1;
	}
	
	fColor = color;
	//fColor = diffuse_material;
	//fColor = vec4(1, 0.41, 0.71, 1);
}