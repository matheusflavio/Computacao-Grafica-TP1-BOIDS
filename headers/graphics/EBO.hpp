#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

// Element Buffer Object - Armazena índices de vértices para renderização eficiente
class EBO {
    public:
        GLuint ID;
        
        // Construtor que cria e preenc o EBO com índices
        EBO(GLuint* vertices, int size);
        
        // Vincula este EBO para edição
        void bind();
        
        // Desvincula este EBO
        void unbind();
        
        // Deleta este EBO
        void deleteIt();
    
        ~EBO();
};

#endif