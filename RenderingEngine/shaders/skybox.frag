#version 430

in vec3 cube_map_coords;

uniform samplerCube cube_map;

layout (location = 0) out vec4 fColor;

const float gamma = 2.2;
const float exposure = 0.5;

void main(void)
{
   vec4 hdrColor = texture(cube_map, cube_map_coords);   
   
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-vec3(hdrColor) * exposure);
    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
	
   fColor = vec4(mapped, 1);
   //fColor = vec4(cube_map_coords, 1);
   //fColor = vec4(1, 0.41, 0.71, 1);
}
