#ifndef vectors_hpp
#define vectors_hpp

#include <stdio.h>
#include <GLFW/glfw3.h>

// Classe que representa um vetor 2D
class Vec2 {
    public:
        float x, y;
        
        // Construtor com valores específicos
        Vec2(float x, float y);
        
        // Construtor padrão (inicializa com zero)
        Vec2();
};

// Classe que representa um vetor 3D (RGB)
class Vec3 {
    public:
        float r, g, b;
        
        // Construtor com valores RGB
        Vec3(float r, float g, float b);
        
        // Construtor padrão (inicializa com zero)
        Vec3();
};

#endif