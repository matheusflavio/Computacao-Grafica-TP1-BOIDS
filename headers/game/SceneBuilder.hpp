#ifndef SCENE_BUILDER_HPP
#define SCENE_BUILDER_HPP

#include <vector>
#include "GObject.hpp"
#include "boid.hpp"
#include "vectors.hpp"

// SceneBuilder: responsável por criar e inicializar todos os objetos da cena.
// Encapsula lógica de criação de walls, obstacles, boids, plane, sky, etc.
class SceneBuilder {
public:
    // Retorno estruturado da criação da cena
    struct Scene {
        GObject* plane;
        GObject* sky;
        std::vector<GObject*> walls;
        std::vector<GObject*> obstacles;
        std::vector<Boid*> boids;
        Boid* ghostBoid;
        Boid* goalBoid;
        v3 towerTop;
    };

    // Parâmetros de configuração da cena
    struct SceneConfig {
        float worldSize;
        float worldHeight;
        int boidGridSize; // número de boids em cada dimensão (ex: 5x5)
    };

    // Cria e inicializa toda a cena com os parâmetros fornecidos
    static Scene buildScene(const SceneConfig& config);
};

#endif
