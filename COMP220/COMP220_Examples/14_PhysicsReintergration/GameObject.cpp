#include "GameObject.h"

GameObject::GameObject()
{
	m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	m_ModelMatrix = glm::mat4(1.0f);

	m_DiffuseMap = 0;

	m_AmbientMaterialColour = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DiffuseMaterialColour = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	m_SpecularMaterialColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpecularPower = 25.0f;

	m_ShaderProgramID = 0;

	m_CollisionShape = nullptr;
	m_Rigidbody = nullptr;
}

GameObject::~GameObject()
{
}

void GameObject::loadMeshesFromFile(const std::string & filename)
{
	
	loadMeshFromFile(filename, m_Meshes);
}

void GameObject::loadDiffuseTextureFromFile(const std::string & filename)
{
	m_DiffuseMap = loadTextureFromFile(filename);
}

void GameObject::loadShaderProgram(const std::string & vertexShaderFilename, const std::string & fragmentShaderFilename)
{
	m_ShaderProgramID = LoadShaders(vertexShaderFilename.c_str(), fragmentShaderFilename.c_str());
}

void GameObject::update()
{
	if (m_Rigidbody != nullptr)
	{
		btTransform transform = m_Rigidbody->getWorldTransform();
		btVector3 origin = transform.getOrigin();
		btQuaternion rotation = transform.getRotation();
		//printf("Position %f\n", carOrigin.getY());

		m_Position = glm::vec3(origin.getX(), origin.getY(), origin.getZ());
	}
	
	glm::mat4 translationMatrix = glm::translate(m_Position);
	glm::mat4 scaleMatrix = glm::scale(m_Scale);
	glm::mat4 rotationMatrix = glm::rotate(m_Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))*
		glm::rotate(m_Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	m_ModelMatrix = translationMatrix*rotationMatrix*scaleMatrix;
}

//Destroys GameObjects
void GameObject::destroy()
{
	if (m_Rigidbody != nullptr)
	{
		delete m_Rigidbody->getMotionState();
		delete m_Rigidbody;
	}

	if (m_CollisionShape != nullptr)
	{
		delete m_CollisionShape;
	}
	glDeleteTextures(1, &m_DiffuseMap);
	glDeleteProgram(m_ShaderProgramID);
	auto iter = m_Meshes.begin();
	while (iter != m_Meshes.end())
	{
		if ((*iter))
		{
			delete (*iter);
			iter = m_Meshes.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

//Rendering Prerequiestes
void GameObject::preRender()
{
	glUseProgram(m_ShaderProgramID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DiffuseMap);

	GLint modelMatrixLocation = glGetUniformLocation(m_ShaderProgramID, "modelMatrix");
	GLint textureLocation = glGetUniformLocation(m_ShaderProgramID, "baseTexture");

	GLint ambientMaterialColourLocation = glGetUniformLocation(m_ShaderProgramID, "ambientMaterialColour");
	GLint diffuseMaterialColourLocation = glGetUniformLocation(m_ShaderProgramID, "diffuseMaterialColour");
	GLint specularMaterialColourLocation = glGetUniformLocation(m_ShaderProgramID, "specularMaterialColour");
	GLint specularPowerLocation = glGetUniformLocation(m_ShaderProgramID, "specularPower");

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(m_ModelMatrix));

	glUniform1i(textureLocation, 0);

	glUniform4fv(ambientMaterialColourLocation, 1, value_ptr(m_AmbientMaterialColour));
	glUniform4fv(diffuseMaterialColourLocation, 1, value_ptr(m_DiffuseMaterialColour));
	glUniform4fv(specularMaterialColourLocation, 1, value_ptr(m_SpecularMaterialColour));
	glUniform1f(specularPowerLocation, m_SpecularPower);
}

//Renders Object
void GameObject::render()
{
	for (Mesh *pMesh : m_Meshes)
	{
		pMesh->render();
	}

}
