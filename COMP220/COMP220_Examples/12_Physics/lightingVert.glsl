#version 330 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec4 vertexColour;
layout(location=2) in vec2 vertexTextureCoord;
layout(location=3) in vec3 vertexNormals;

uniform float time=0.0f;

uniform mat4 modelMatrix=mat4(1.0f);
uniform mat4 viewMatrix=mat4(1.0f);
uniform mat4 projectionMatrix=mat4(1.0f);

//Camera position
uniform vec3 cameraPosition;

//Lighting
uniform vec3 lightDirection;
uniform vec4 ambientLightColour;
uniform vec4 diffuseLightColour;
uniform vec4 specularLightColour;

//Material
uniform vec4 ambientMaterialColour;
uniform vec4 diffuseMaterialColour;
uniform vec4 specularMaterialColour;
uniform float specularPower;

out vec4 vertexColourOut;
out vec2 vertexTextureCoordOut;
out vec4 diffuse;
out vec4 specular;
out vec4 ambient;

void main()
{
	mat4 MVPMatrix=projectionMatrix*viewMatrix*modelMatrix;
	vec4 worldNormals=normalize(modelMatrix*vec4(vertexNormals,0.0f));
	
	//World position of vertex
	vec4 worldPosition=modelMatrix*vec4(vertexPosition,1.0f);

	vec3 viewDirection=normalize(cameraPosition-worldPosition.xyz);

	//calculate ambient
	ambient=ambientMaterialColour*ambientLightColour;

	//Calculate Diffuse lighting
	float nDotl=clamp(dot(worldNormals.xyz,lightDirection),0,1);
	diffuse=diffuseMaterialColour*diffuseLightColour*nDotl;

	//Calculate Specular lighting
	vec3 halfWay=normalize(lightDirection+viewDirection);
	float nDoth=clamp(dot(worldNormals.xyz,halfWay),0,1);
	float specularInstensity=pow(nDoth,specularPower);
	specular=specularMaterialColour*specularLightColour*specularInstensity;
	

	gl_Position=MVPMatrix*vec4(vertexPosition,1.0f);
	vertexColourOut=vertexColour;
	vertexTextureCoordOut=vertexTextureCoord;
}