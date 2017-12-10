#version 330 core

layout(location=0) in vec2 vertexPosition;

out vec2 textureCoordsOut;

void main()
{
	gl_Position=vec4(vertexPosition,0.0f,1.0f);
	textureCoordsOut=(vertexPosition+1.0f)/2.0f;
}