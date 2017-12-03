#version 330 core

in vec4 vertexColourOut;
in vec2 vertexTextureCoordOut;
in vec4 diffuse;
in vec4 specular;
in vec4 ambient;

out vec4 colour;

uniform vec4 fragColour=vec4(1.0,1.0,1.0,1.0);

uniform sampler2D baseTexture;

void main()
{
	colour=ambient+(diffuse*texture(baseTexture,vertexTextureCoordOut))+specular;
}