#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.hpp"

// Vertex Array Object - Encapsula a configuração de atributos de vértices
class VAO{
    public:
        GLuint ID;
        
        // Construtor que gera um identificador VAO
        VAO();
        
        // Vincula um atributo de vértice ao VBO
        void linkAttribute(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
        
        // Vincula este VAO para edição
        void bind();
        
        // Desvincula este VAO
        void unbind();
        
        // Deleta este VAO
        void deleteIt();
        
        // Libera recursos
        void destroy();
        
        ~VAO();
};

#endif