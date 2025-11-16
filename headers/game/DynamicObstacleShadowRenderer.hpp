#ifndef DYNAMIC_OBSTACLE_SHADOW_RENDERER_HPP
#define DYNAMIC_OBSTACLE_SHADOW_RENDERER_HPP

#include <vector>
#include "GObject.hpp"
#include "shaderClass.hpp"
#include "vectors.hpp"

// DynamicObstacleShadowRenderer: renderiza sombras dinâmicas dos obstáculos
// durante a preview de luz, utilizando ray-plane intersection.
// As sombras dinâmicas são calculadas e projetadas em superfícies planas.
class DynamicObstacleShadowRenderer {
public:
    // Renderiza sombras dinâmicas dos obstáculos baseadas na direção da luz.
    // Projeta obstáculos em superfícies (plane, walls) ao longo da direção de luz.
    static void renderDynamicShadows(
        const std::vector<GObject*>& obstacles,
        const std::vector<GObject*>& surfaces, // plane + walls
        const v3& lightDirection,
        Shader* shader
    );
};

#endif
