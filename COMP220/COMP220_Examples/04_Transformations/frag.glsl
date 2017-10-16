#version 330 core

out vec4 colour;

uniform vec4 fragColour=vec4(1.5,1.5,1.5,2.0);

void main()
{
	colour=fragColour;
}