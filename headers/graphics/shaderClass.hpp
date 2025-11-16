#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Classe para gerenciar programas de shader OpenGL
class Shader
{
public:
	// Identificador do programa de shader
	unsigned int id;
	
	// Construtor padrão (cria shader vazio)
	Shader();
	
	// Construtor que lê e compila os shaders vertex e fragment
	Shader(const char* vertexPath, const char* fragmentPath);
	
	// Ativa este programa de shader para uso
	void activate();
	
	// Funções utilitárias para definir valores de uniforms no programa
	void setBool(const std::string &uniformName, bool value) const;
	void setInt(const std::string &uniformName, int value) const;
	void setFloat(const std::string &uniformName, float value) const;
	void setVec4(const std::string &uniformName, float value[4]) const;
	void setVec4(const std::string &uniformName, const glm::vec4 &value) const;
	void setMat4(const std::string &uniformName, const glm::mat4 &value) const;

    // Deleta o programa de shader
    void deleteIt();
};
#endif