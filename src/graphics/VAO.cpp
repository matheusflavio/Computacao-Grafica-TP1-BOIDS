#include "VAO.hpp"
#include <iostream>
#include "globals.hpp"

// Construtor que gera um VAO ID
VAO::VAO(){
    glGenVertexArrays(1, &ID);
}

// Vincula um atributo de vértice ao VBO
void VAO::linkAttribute(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset){
    vbo.bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, (int)stride, offset);
    glEnableVertexAttribArray(layout);
    vbo.unbind();
}

// Vincula o VAO
void VAO::bind(){
    glBindVertexArray(ID);
}

// Desvincula o VAO
void VAO::unbind(){
    glBindVertexArray(0);
}

// Deleta o VAO
void VAO::deleteIt(){
    glDeleteVertexArrays(1, &ID);
}

// Libera recursos do VAO
void VAO::destroy() {
    if(debugLevel == DEBUG_ALL) std::cout << "VAO destruído" << std::endl;
    unbind();
    deleteIt();
}

VAO::~VAO() {
    destroy();
}
