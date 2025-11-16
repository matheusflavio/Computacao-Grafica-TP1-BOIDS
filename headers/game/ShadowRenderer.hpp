#ifndef SHADOW_RENDERER_HPP
#define SHADOW_RENDERER_HPP

#include <vector>
#include "boid.hpp"
#include "shaderClass.hpp"

// ShadowRenderer: responsável por computar e desenhar sombras simples no chão (y=0).
// Comentários em português conforme convenção do projeto.
class ShadowRenderer {
public:
    // Renderiza sombras projetadas no plano y=0 para a lista de boids.
    // Usa um único VBO/EBO para todos os boids (batching) para manter a performance.
    static void renderGroundShadows(const std::vector<Boid*>& boids, Shader* shader);
};

#endif
