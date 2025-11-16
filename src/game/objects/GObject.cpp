
#include "GObject.hpp"
#include <glm/glm.hpp>
typedef glm::vec3 v3;

// Construtor padrão que inicializa listas vazias
GObject::GObject() {
    this->vertices = vector<GVertice>();
    this->indices = vector<int>();
}

// Adiciona um vértice ao objeto
int GObject::addVertice(GVertice v) {
    vertices.push_back(v);
    return vertices.size() - 1;
}

// Adiciona um triângulo definido por três índices de vértices
void GObject::addTriangle(int v1, int v2, int v3) {
    indices.push_back(v1);
    indices.push_back(v2);
    indices.push_back(v3);

    verticesGroup.push_back(vector<int>({v1, v2, v3}));
}

// Move o objeto no espaço
void GObject::translate(v3 translation) {
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].coords += translation;
    }
}

// Retorna o tamanho em floats (11 por vértice)
int GObject::getSize() {
    return vertices.size() * 11;
}

// Anima o objeto (pode ser sobrescrito por subclasses)
void GObject::animate() {}

// Recalcula as normais de todos os vértices
void GObject::recalculateNormals() {
    if(!generateNormals) return;
    
    // Para cada triângulo
    for(int i = 0; i < verticesGroup.size(); i++) {
        // Obtém os três vértices do triângulo
        v3 ve1 = vertices[verticesGroup[i][0]].coords;
        v3 ve2 = vertices[verticesGroup[i][1]].coords;
        v3 ve3 = vertices[verticesGroup[i][2]].coords;

        // Calcula a normal usando produto vetorial
        v3 normal = glm::normalize(glm::cross(ve2 - ve1, ve3 - ve1));

        // Adiciona a normal a cada vértice do triângulo
        vertices[verticesGroup[i][0]].normal += normal;
        vertices[verticesGroup[i][1]].normal += normal;
        vertices[verticesGroup[i][2]].normal += normal;
    }

    // Normaliza as normais de todos os vértices
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
}

void GObject::prepare(GLfloat* vArray, long &arrayPos, GLuint* iArray, long &indicesPos) {
    if(destroyed) return;
    
    // Calcula o ID inicial baseado na posição no array
    // Cada vértice ocupa 12 posições (coords 3 + cores 3 + opacity 1 + texture 2 + normal 3)
    int startingId = (int) arrayPos/12;

    // Adiciona todos os vértices ao array
    for(int i = 0; i < vertices.size(); i++) {
        vArray[arrayPos++] = vertices[i].coords.x;
        vArray[arrayPos++] = vertices[i].coords.y;
        vArray[arrayPos++] = vertices[i].coords.z;
        vArray[arrayPos++] = vertices[i].colors.x;
        vArray[arrayPos++] = vertices[i].colors.y;
        vArray[arrayPos++] = vertices[i].colors.z;
        vArray[arrayPos++] = opacity;
        vArray[arrayPos++] = vertices[i].texture.x;
        vArray[arrayPos++] = vertices[i].texture.y;
        vArray[arrayPos++] = vertices[i].normal.x;
        vArray[arrayPos++] = vertices[i].normal.y;
        vArray[arrayPos++] = vertices[i].normal.z;
    }
    
    // Adiciona todos os índices ao array, ajustando com o ID inicial
    for(int i = 0; i < indices.size(); i++) {
        iArray[indicesPos++] = indices[i] + startingId;
    }
}

// Atualiza o estado do objeto a cada frame
void GObject::frameUpdate() {
    // Move o objeto baseado no vetor de velocidade
    translate(this->speedVector * this->speed * this->speedMultiplier);
    
    // Recalcula as normais para iluminação correta
    recalculateNormals();
}

// Redimensiona o objeto (escala)
void GObject::scale(v3 scale) {
    for(int i = 0; i < vertices.size(); i++) {
        vertices[i].coords.x *= scale.x;
        vertices[i].coords.y *= scale.y;
        vertices[i].coords.z *= scale.z;
    }
}

// Rotaciona o objeto em graus
void GObject::rotate(v3 degrees) {
    v3 origin = vertices[0].coords;
    for(int i = 0; i < vertices.size(); i++) {
        // Centraliza no ponto de origem
        vertices[i].coords -= origin;
        
        // Rotaciona em cada eixo
        vertices[i].coords = glm::rotate(vertices[i].coords, glm::radians(degrees.x), v3(1, 0, 0));
        vertices[i].coords = glm::rotate(vertices[i].coords, glm::radians(degrees.y), v3(0, 1, 0));
        vertices[i].coords = glm::rotate(vertices[i].coords, glm::radians(degrees.z), v3(0, 0, 1));
        
        // Retorna ao ponto original
        vertices[i].coords += origin;
    }
}

v3 GObject::getPosition() {
    return vertices[0].coords;
}

void GObject::destroy() {
    destroyed = true;
}