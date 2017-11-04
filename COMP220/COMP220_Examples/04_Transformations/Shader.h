#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <GL\glew.h>
#include <SDL_opengl.h>

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);