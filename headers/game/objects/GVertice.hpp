#ifndef GVERTICE_CLASS_H
#define GVERTICE_CLASS_H

#include <glm/glm.hpp>

using namespace std;
typedef glm::vec3 v3;
typedef glm::vec2 v2;

// Classe que representa um vértice com seus atributos
class GVertice {
    public:
        // Coordenadas do vértice no espaço 3D
        v3 coords;
        
        // Cor do vértice (RGB)
        v3 colors;
        
        // Coordenadas de textura (UV)
        v2 texture;
        
        // Normal do vértice para cálculos de iluminação
        v3 normal;
        
        // Construtor completo com todos os atributos
        GVertice(v3 coords, v3 colors, v2 texture, v3 normal);
        
        // Construtor sem normal (usa padrão zero)
        GVertice(v3 coords, v3 colors, v2 texture);
};

#endif