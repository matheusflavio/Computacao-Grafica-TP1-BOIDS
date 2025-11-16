#include "EBO.hpp"
#include "globals.hpp"
#include <iostream>

// Constructor that generates a Elements Buffer Object and links it to indices
EBO::EBO(GLuint* indices, int size){
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void EBO::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::deleteIt(){
    glDeleteBuffers(1, &ID);
}

EBO::~EBO() {
    unbind();
    if(debugLevel == DEBUG_ALL) std::cout << "EBO destroy" << std::endl;
    deleteIt();
}
