#include "Model.h"

bool loadModelFromFile(const std::string& filename, GLuint VBO, GLuint EBO, unsigned int& numberOfVerts, unsigned int& numberIndices)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs);
	
	if (scene == nullptr)
	{
		printf("Error Loading model! %s", importer.GetErrorString());
		return false;
	}
	
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//Scans through all of the meshes in the scene
	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		const aiMesh* currentAIMesh = scene->mMeshes[m];
		for (unsigned int v = 0; v < currentAIMesh->mNumVertices; v++)
		{
			const aiVector3D currentAIPosition = currentAIMesh->mVertices[v];

			Vertex ourVertex;
			ourVertex.x = currentAIPosition.x;
			ourVertex.y = currentAIPosition.y;
			ourVertex.z = currentAIPosition.z;
			ourVertex.r = 1.0f; ourVertex.g = 1.0f; ourVertex.b = 1.0f;
			ourVertex.tu = 0.0f; ourVertex.tv = 0.0f;
			
			if (currentAIMesh->HasTextureCoords(0));
			{
				const aiVector3D currentTextureCoords = currentAIMesh->mTextureCoords[0][v];
				ourVertex.tu = currentTextureCoords.x;
				ourVertex.tv = currentTextureCoords.y;
			}
			
			if (currentAIMesh->HasVertexColors(0));
			{
				const aiColor4D currentColour = currentAIMesh->mColors[0][v];
				ourVertex.r = currentColour.r;
				ourVertex.g = currentColour.g;
				ourVertex.b = currentColour.b;
				ourVertex.a = currentColour.a;
			}
			
			
			//Stores Vertices
			vertices.push_back(ourVertex);
		}
		//Gets faces
		for (unsigned int f = 0; f < currentAIMesh->mNumFaces; f++)
		{
			const aiFace currentFace = currentAIMesh->mFaces[f];
			
			indices.push_back(currentFace.mIndices[0]);
			indices.push_back(currentFace.mIndices[1]);
			indices.push_back(currentFace.mIndices[2]);
		}
	
	
	
	}
	
	
	numberOfVerts = vertices.size();
	numberIndices = indices.size();
	//Copies Data from "HOLD" to the draw buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numberOfVerts * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numberIndices * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	return true;

}

bool loadMeshFromFile(const std::string & filename, std::vector<Mesh*>& meshes)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_FlipUVs);

	if (scene == nullptr)
	{
		printf("Error Loading model! %s", importer.GetErrorString());
		return false;
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//Scans through all of the meshes in the scene
	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		const aiMesh* currentAIMesh = scene->mMeshes[m];
		
		
		//Creating a copy of the encountered meshes
		Mesh * ourCurrentMesh = new Mesh();
		ourCurrentMesh->init();
		
		for (unsigned int v = 0; v < currentAIMesh->mNumVertices; v++)
		{
			const aiVector3D currentAIPosition = currentAIMesh->mVertices[v];

			Vertex ourVertex;
			ourVertex.x = currentAIPosition.x;
			ourVertex.y = currentAIPosition.y;
			ourVertex.z = currentAIPosition.z;
			ourVertex.r = 1.0f; ourVertex.g = 1.0f; ourVertex.b = 1.0f;
			ourVertex.tu = 0.0f; ourVertex.tv = 0.0f;

			if (currentAIMesh->HasTextureCoords(0));
			{
				const aiVector3D currentTextureCoords = currentAIMesh->mTextureCoords[0][v];
				ourVertex.tu = currentTextureCoords.x;
				ourVertex.tv = currentTextureCoords.y;
			}

			if (currentAIMesh->HasVertexColors(0));
			{
				const aiColor4D currentColour = currentAIMesh->mColors[0][v];
				ourVertex.r = currentColour.r;
				ourVertex.g = currentColour.g;
				ourVertex.b = currentColour.b;
				ourVertex.a = currentColour.a;
			}


			//Stores Vertices
			vertices.push_back(ourVertex);
		}
		//Gets faces
		for (unsigned int f = 0; f < currentAIMesh->mNumFaces; f++)
		{
			const aiFace currentFace = currentAIMesh->mFaces[f];

			indices.push_back(currentFace.mIndices[0]);
			indices.push_back(currentFace.mIndices[1]);
			indices.push_back(currentFace.mIndices[2]);
		}

		ourCurrentMesh->copyMeshData(vertices, indices);
		meshes.push_back(ourCurrentMesh);

		vertices.clear();
		indices.clear();
	}
	
	
	return true;
}
