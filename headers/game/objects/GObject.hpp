#ifndef GOBJECT_CLASS_H
#define GOBJECT_CLASS_H

#include <glm/glm.hpp>
#include <vector>
#include "GVertice.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace std;
typedef glm::vec3 v3;
typedef glm::vec2 v2;

// Classe base para objetos gráficos 3D
class GObject {
    public:
        // Lista de vértices do objeto
        vector<GVertice> vertices;
        
        // Lista de índices para renderização (define triângulos)
        vector<int> indices;
        
        // Flag indicando se o objeto foi destruído
        bool destroyed = false;

        // Adiciona um vértice ao objeto, retorna seu índice
        int addVertice(GVertice v);
        
        // Adiciona um triângulo definido por três índices de vértices
        void addTriangle(int v1, int v2, int v3);

        // Move o objeto no espaço
        void translate(v3 translation);

        // Retorna o número de vértices
        int getSize();

        // Prepara dados para renderização em arrays
        void prepare(GLfloat *array, long &arrayPos, GLuint *indices, long &indicesPos);
        
        // Redimensiona o objeto proporcionalmente
        void scale(v3 scale);
        
        // Rotaciona o objeto em graus
        void rotate(v3 rotation);

        // Vetor de velocidade para movimento
        v3 speedVector;
        
        // Velocidade de movimento
        float speed = 1.0f;
        
        // Multiplicador de velocidade
        float speedMultiplier = 1.0f;

        // Chamado a cada frame para animar o objeto
        virtual void animate();
        
        // Chamado a cada frame para atualizar estado
        virtual void frameUpdate();
        
        // Recalcula as normais de todos os vértices
        void recalculateNormals();

        // Marca o objeto como destruído
        void destroy();

        // Construtor padrão
        GObject();

        // Retorna a posição do objeto (primeira vértice)
        virtual v3 getPosition();

        // Grupo de vértices (triângulos)
        vector<vector<int>> verticesGroup;

        // Se as normais devem ser geradas automaticamente
        bool generateNormals = false;
        
        // Opacidade do objeto (0.0 - 1.0)
        float opacity = 1.0;
     
};

#endif