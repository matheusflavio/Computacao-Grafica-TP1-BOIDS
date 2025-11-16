#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

// Vertex Buffer Object - Armazena dados de vértices na GPU
class VBO {
    public:
        GLuint ID;
        
        // Construtor que cria e preenc o VBO com dados de vértices
        VBO(GLfloat* vertices, int size);
        
        // Vincula este VBO para edição
        void bind();
        
        // Desvincula este VBO
        void unbind();
        
        // Deleta este VBO
        void deleteIt();
    
        ~VBO();
};

#endif