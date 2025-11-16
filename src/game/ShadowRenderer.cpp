#include <glad/glad.h>
#include "ShadowRenderer.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "GObject.hpp"
#include "vectors.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// Comentários em português: implementação do renderer de sombras
void ShadowRenderer::renderGroundShadows(const std::vector<Boid*>& boids, Shader* shader) {
    // Junta todos os vértices/índices dos boids em buffers únicos (batch)
    int totalVertices = 0;
    int totalIndices = 0;
    for (size_t i = 0; i < boids.size(); ++i) {
        if (boids[i] == NULL || boids[i]->destroyed) continue;
        totalVertices += boids[i]->vertices.size();
        totalIndices += boids[i]->indices.size();
    }
    if (totalVertices == 0 || totalIndices == 0) return;

    int sizeVArray = totalVertices * 12 * sizeof(GLfloat);
    int sizeIArray = totalIndices * sizeof(GLuint);

    // Usa std::vector ao invés de malloc/free para RAII e segurança
    std::vector<GLfloat> vArray(totalVertices * 12);
    std::vector<GLuint> iArray(totalIndices);

    long arrayPos = 0;
    long indicesPos = 0;
    int startingId = 0;
    for (size_t b = 0; b < boids.size(); ++b) {
        GObject *obj = boids[b];
        if (obj == NULL || obj->destroyed) continue;

        v3 center = obj->getPosition();
        float wingStretchFactor = 0.08f; // controla espalhamento das asas na sombra

        for (size_t i = 0; i < obj->vertices.size(); ++i) {
            v3 c = obj->vertices[i].coords;
            float shadowY = 0.05f; // pequeno offset para evitar z-fighting
            float yDelta = c.y - center.y;
            float factor = 1.0f + (yDelta * wingStretchFactor);
            if (factor < 0.6f) factor = 0.6f;
            if (factor > 1.4f) factor = 1.4f;
            float projX = center.x + (c.x - center.x) * factor;
            float projZ = center.z + (c.z - center.z) * factor;

            vArray[arrayPos++] = projX;
            vArray[arrayPos++] = shadowY;
            vArray[arrayPos++] = projZ;
            // cor preta
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.0f;
            // opacidade
            vArray[arrayPos++] = 0.45f;
            // texcoords (não usados)
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 0.0f;
            // normal apontando pra cima
            vArray[arrayPos++] = 0.0f;
            vArray[arrayPos++] = 1.0f;
            vArray[arrayPos++] = 0.0f;
        }

        for (size_t i = 0; i < obj->indices.size(); ++i) {
            iArray[indicesPos++] = obj->indices[i] + startingId;
        }
        startingId += obj->vertices.size();
    }

    // Cria VAO/VBO/EBO temporários e desenha (um draw call batched)
    VAO shadowVAO;
    shadowVAO.bind();
    VBO shadowVBO(vArray.data(), sizeVArray);
    EBO shadowEBO(iArray.data(), sizeIArray);

    shadowVAO.linkAttribute(shadowVBO, 0, 3, GL_FLOAT, 12 * sizeof(float), (void*)0);
    shadowVAO.linkAttribute(shadowVBO, 1, 4, GL_FLOAT, 12 * sizeof(float), (void*)(3 * sizeof(float)));
    shadowVAO.linkAttribute(shadowVBO, 2, 2, GL_FLOAT, 12 * sizeof(float), (void*)(7 * sizeof(float)));
    shadowVAO.linkAttribute(shadowVBO, 3, 3, GL_FLOAT, 12 * sizeof(float), (void*)(9 * sizeof(float)));

    shader->activate();
    glUniform1i(glGetUniformLocation(shader->id, "u_isShadow"), 1);

    shadowVAO.bind();
    glDrawElements(GL_TRIANGLES, sizeIArray / sizeof(int), GL_UNSIGNED_INT, 0);

    shadowVAO.unbind();
    shadowVBO.unbind();
    shadowEBO.unbind();
    glUniform1i(glGetUniformLocation(shader->id, "u_isShadow"), 0);
    
    // std::vector limpa automaticamente ao sair do escopo (RAII)
}
