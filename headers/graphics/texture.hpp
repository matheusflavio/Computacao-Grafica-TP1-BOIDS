#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include "shaderClass.hpp"

// Classe para gerenciar texturas OpenGL
class Texture
{
public:
	GLuint ID;
	GLenum type;
	
	// Construtor que carrega uma textura de arquivo
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	// Vincula uma unidade de textura a um uniforme do shader
	void bindUnit(Shader& shader, const char* uniform, GLuint unit);
	
	// Vincula esta textura para uso
	void bind();
	
	// Desvincula esta textura
	void unbind();
	
	// Deleta os recursos desta textura
	void deleteIt();
};
#endif