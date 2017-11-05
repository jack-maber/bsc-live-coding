#pragma once
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <string>
#include <GL\glew.h>
#include <SDL_opengl.h>
#include <vector>
#include "Vertex.h"
#include "Mesh.h"


bool loadModelFromFile(const std::string & filename, GLuint VBO, GLuint EBO, unsigned int & numberOfVerts, unsigned int & numberIndices);

bool loadMeshFromFile(const std::string& filename, std::vector<Mesh*>& meshes);