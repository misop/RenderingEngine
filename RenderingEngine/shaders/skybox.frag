#version 430

in vec3 cube_map_coords;

uniform samplerCube cube_map;

layout (location = 0) out vec4 fColor;

void main(void)
{
   fColor = texture(cube_map, cube_map_coords);
   //fColor = vec4(cube_map_coords, 1);
   //fColor = vec4(1, 0.41, 0.71, 1);
}
