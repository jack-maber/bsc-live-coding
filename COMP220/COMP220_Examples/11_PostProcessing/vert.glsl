#version 330 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec4 vertexColour;

uniform float time=0.0f;

uniform mat4 modelMatrix=mat4(1.0f);
uniform mat4 viewMatrix=mat4(1.0f);
uniform mat4 projectionMatrix=mat4(1.0f);

out vec4 vertexColourOut;

void main()
{
	mat4 MVPMatrix=projectionMatrix*viewMatrix*modelMatrix;

	gl_Position=MVPMatrix*vec4(vertexPosition,1.0f);
	vertexColourOut=vertexColour;
}