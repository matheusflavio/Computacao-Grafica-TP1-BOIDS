#ifndef GText_hpp
#define GText_hpp

#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <iostream>
#include <string>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "shaderClass.hpp"

using namespace std;

// Estrutura que armazena informações sobre um caractere de fonte
struct Character {
    unsigned int TextureID;  // ID da textura do glifo
    glm::ivec2   Size;       // Tamanho do glifo
    glm::ivec2   Bearing;    // Deslocamento da base até o topo/esquerda do glifo
    unsigned int Advance;    // Deslocamento para o próximo glifo
    unsigned int VAO, VBO;
};

// Classe para renderização de texto 2D usando FreeType
class GText{
public:
    GText();
    ~GText();
    // Carrega as fontes
    void loadFonts();
    
    // Renderiza texto na tela
    void renderText(string text, float x, float y, float scale, glm::vec3 color);
    
    // Vertex Array Object e Vertex Buffer Object para renderização de texto
    unsigned int VAO, VBO;
    
    // Shader para renderização de texto
    Shader textShader;
};

#endif