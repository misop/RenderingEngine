#version 430

layout(location = 0) in vec2 uv;


layout(binding = 0) uniform sampler2D hdrBuffer;

const float gamma = 2.0;
const float exposure = 1e-7;

layout(location = 0) out vec4 fColor;

void main(void)
{
    vec4 hdrColor = texture(hdrBuffer, uv).rgba;
	
	//vec3 mapped = vec3(hdrColor) / 1e6;
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-vec3(hdrColor) * exposure);
    // Gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    fColor = vec4(mapped, hdrColor.a);
	//fColor = hdrColor;
}