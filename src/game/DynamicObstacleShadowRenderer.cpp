#include <glad/glad.h>
#include "DynamicObstacleShadowRenderer.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "vectors.hpp"
#include <glm/glm.hpp>
#include <vector>

// Renderiza sombras dos obstáculos projetadas no chão (y=0) e paredes
// Usa batching para eficiência: todos os obstáculos em um único draw call
void DynamicObstacleShadowRenderer::renderDynamicShadows(
    const std::vector<GObject*>& obstacles,
    const std::vector<GObject*>& surfaces,
    const v3& lightDirection,
    Shader* shader
) {
    if (obstacles.empty()) return;
    
    // Coleta todos os vértices e índices dos obstáculos para batching
    int totalVertices = 0;
    int totalIndices = 0;
    
    for (const auto& obstacle : obstacles) {
        if (!obstacle || obstacle->destroyed) continue;
        totalVertices += obstacle->vertices.size();
        totalIndices += obstacle->indices.size();
    }
    
    if (totalVertices == 0 || totalIndices == 0) return;
    
    // Usa std::vector para segurança RAII (evita memory leaks)
    std::vector<GLfloat> vArray(totalVertices * 12);
    std::vector<GLuint> iArray(totalIndices);
    
    int arrayPos = 0;
    int indicesPos = 0;
    int startingId = 0;
    
    // Preenche buffers: projeta cada vértice do obstáculo no chão (y=0)
    for (const auto& obstacle : obstacles) {
        if (!obstacle || obstacle->destroyed) continue;
        
        for (size_t i = 0; i < obstacle->vertices.size(); ++i) {
            v3 c = obstacle->vertices[i].coords;
            
            // Projeção no chão com pequeno offset (y=0.05) para evitar z-fighting
            vArray[arrayPos++] = c.x;
            vArray[arrayPos++] = 0.05f;
            vArray[arrayPos++] = c.z;
            
            // cor preta com opacidade (RGBA)
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.6f;
            
            // texcoords (não usados)
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.0f;
            
            // normal apontando para cima
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 1.0f;
            vArray[arrayPos++] = 0.0f;
        }
        
        // Adiciona índices com offset
        for (size_t i = 0; i < obstacle->indices.size(); ++i) {
            iArray[indicesPos++] = obstacle->indices[i] + startingId;
        }
        
        startingId += obstacle->vertices.size();
    }
    
    // Cria e desenha VAO/VBO/EBO (batching: um único draw call para todos os obstáculos)
    VAO shadowVAO;
    shadowVAO.bind();
    
    VBO shadowVBO(vArray.data(), vArray.size() * sizeof(GLfloat));
    EBO shadowEBO(iArray.data(), iArray.size() * sizeof(GLuint));
    
    // Layout: pos(3) | color(4) | texcoords(2) | normal(3)
    shadowVAO.linkAttribute(shadowVBO, 0, 3, GL_FLOAT, 12 * sizeof(float), (void*)0);
    shadowVAO.linkAttribute(shadowVBO, 1, 4, GL_FLOAT, 12 * sizeof(float), (void*)(3 * sizeof(float)));
    shadowVAO.linkAttribute(shadowVBO, 2, 2, GL_FLOAT, 12 * sizeof(float), (void*)(7 * sizeof(float)));
    shadowVAO.linkAttribute(shadowVBO, 3, 3, GL_FLOAT, 12 * sizeof(float), (void*)(9 * sizeof(float)));
    
    shader->activate();
    
    // Ativa flag de sombra no shader (renderiza com cor + transparência, sem iluminação)
    glUniform1i(glGetUniformLocation(shader->id, "u_isShadow"), 1);
    
    shadowVAO.bind();
    glDrawElements(GL_TRIANGLES, iArray.size(), GL_UNSIGNED_INT, 0);
    
    // Desativa flag de sombra
    glUniform1i(glGetUniformLocation(shader->id, "u_isShadow"), 0);
    
    // Limpeza automática de vArray, iArray (std::vector destrutor)
}
