#pragma once

#include <vector>
#include <string>

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Shader.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void loadMeshesFromFile(const std::string& filename);
	void loadDiffuseTextureFromFile(const std::string& filename);
	void loadShaderProgram(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);

	void update();
	void destroy();
	void preRender();
	void render();

	//Sets the position of the game object
	void setPosition(const glm::vec3& position)
	{
		m_Position = position;
	};

	//gets the position
	const glm::vec3& getPosition()
	{
		return m_Position;
	};


	void setScale(const glm::vec3& scale)
	{
		m_Scale = scale;
	};

	const glm::vec3& getScale()
	{
		return m_Scale;
	};

	void setRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;
	};

	const glm::vec3& getRotation()
	{
		return m_Rotation;
	};

	const glm::mat4& getModelMatrix()
	{
		return m_ModelMatrix;
	};

	const GLuint getDiffuseMap()
	{
		return m_DiffuseMap;
	};


	void setAmbientMaterialColour(const glm::vec4& colour)
	{
		m_AmbientMaterialColour = colour;
	};

	const glm::vec4& getAmbientMaterialColour()
	{
		return m_AmbientMaterialColour;
	};

	void setDiffuseMaterialColour(const glm::vec4& colour)
	{
		m_DiffuseMaterialColour = colour;
	};

	const glm::vec4& getDiffuseMaterialColour()
	{
		return m_DiffuseMaterialColour;
	};

	void setSpecularMaterialColour(const glm::vec4& colour)
	{
		m_SpecularMaterialColour = colour;
	};

	const glm::vec4& getSpecularMaterialColour()
	{
		return m_SpecularMaterialColour;
	};

	void setSpecularPower(const float power)
	{
		m_SpecularPower = power;
	};

	const float getSpecularPower()
	{
		return m_SpecularPower;
	};

	const GLuint getShaderPorgramID()
	{
		return m_ShaderProgramID;
	};

private:
	//The visible mesh
	std::vector<Mesh*> m_Meshes;

	//Transform
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
	glm::mat4 m_ModelMatrix;

	//Textures
	GLuint m_DiffuseMap;

	//Material
	glm::vec4 m_AmbientMaterialColour;
	glm::vec4 m_DiffuseMaterialColour;
	glm::vec4 m_SpecularMaterialColour;
	float m_SpecularPower;

	GLuint m_ShaderProgramID;
};