#include "VBO.hpp"
#include <iostream>
#include "globals.hpp"

VBO::VBO(GLfloat* vertices, int size)
{
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::bind(){
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::deleteIt(){
    glDeleteBuffers(1, &ID);
}

VBO::~VBO() {
    unbind();
    if(debugLevel == DEBUG_ALL) std::cout << "VBO delete buffers" << std::endl;
    deleteIt();
}
