#version 430

layout(location = 0) in vec2 uv;


layout(binding = 0) uniform sampler2D textureA;
layout(binding = 1) uniform sampler2D textureB;

layout(location = 0) out vec4 fColor;

void main(void)
{
    vec4 colorA = texture(textureA, uv).rgba;
    vec4 colorB = texture(textureB, uv).rgba;
  
	fColor = colorA + colorB;
	fColor.a = min(1, fColor.a);
}