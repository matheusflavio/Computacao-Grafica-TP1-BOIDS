#include "SceneBuilder.hpp"
#include "customObjects.hpp"

SceneBuilder::Scene SceneBuilder::buildScene(const SceneBuilder::SceneConfig& config) {
    Scene scene;
    
    // Cria paredes (walls)
    GObject *wall = createWall();
    GObject *wall2 = createWall();
    GObject *wall3 = createWall();
    GObject *wall4 = createWall();
    
    wall2->rotate(glm::vec3(0.0f, 270, 0.0f));
    wall4->rotate(glm::vec3(0.0f, 270, 0.0f));
    wall3->translate(glm::vec3(0.0f, 0.0f, config.worldSize));
    wall4->translate(glm::vec3(config.worldSize, 0.0f, 0.0f));
    
    scene.walls.push_back(wall);
    scene.walls.push_back(wall2);
    scene.walls.push_back(wall3);
    scene.walls.push_back(wall4);
    
    // Cria plano e céu
    scene.plane = createPlane();
    scene.sky = createPlane(v3(3, 173, 252), 1.0f);
    scene.sky->translate(glm::vec3(0.0f, config.worldHeight, 0.0f));
    
    // Cria torre principal e posiciona
    GObject *tower = createTower(70.0f, 20.0f);
    tower->translate(glm::vec3((config.worldSize/2.0f), 0.0f, (config.worldSize/2.0f)));
    scene.obstacles.push_back(tower);
    scene.towerTop = v3(config.worldSize/2.0f, 70.0f, config.worldSize/2.0f);
    
    // Cria torres secundárias
    GObject *t2 = createTower(15.0f, 30.0f);
    GObject *t3 = createTower(45.0f, 15.0f);
    GObject *t4 = createTower(20.0f, 8.0f);
    GObject *t5 = createTower(60.0f, 24.0f);
    GObject *t6 = createTower(30.0f, 5.0f);
    
    t2->translate(glm::vec3(100.0, 0.0f, 50.0f));
    t3->translate(glm::vec3(50.0, 0.0f, 100.0f));
    t4->translate(glm::vec3(200.0, 0.0f, 50.0f));
    t5->translate(glm::vec3(50.0, 0.0f, 200.0f));
    t6->translate(glm::vec3(300.0, 0.0f, 200.0f));
    
    scene.obstacles.push_back(t2);
    scene.obstacles.push_back(t3);
    scene.obstacles.push_back(t4);
    scene.obstacles.push_back(t5);
    scene.obstacles.push_back(t6);
    
    // Cria ghost boid (guia invisível do grupo)
    scene.ghostBoid = new Boid(v3(100.0f, 30.0f, 100.0f), v3(1.0f, 0.0f, 0.0f));
    scene.ghostBoid->opacity = 0.8f;
    scene.ghostBoid->speedMultiplier = 1.06f;
    scene.ghostBoid->translate(v3(30.0f, rand()%6 - 3, 30.0f));
    scene.ghostBoid->scale(v3(0.75f, 0.75f, 0.75f));
    scene.boids.push_back(scene.ghostBoid);
    
    // Cria grid de boids
    for(int i = 1; i <= config.boidGridSize; i++) {
        for(int j = 1; j <= config.boidGridSize; j++) {
            GObject *boid = new Boid(v3(20.0f * j, 30.0f, 20.0f * i));
            boid->translate(v3(0.0f, rand()%6 - 3, 0.0f));
            scene.boids.push_back(dynamic_cast<Boid*>(boid));
        }
    }
    
    // Cria boid objetivo (goal)
    scene.goalBoid = new Boid(v3(0.0f, 30.0f, 0.0f), v3(0.0f, 0.5f, 0.0f));
    scene.goalBoid->opacity = 0.6f;
    scene.goalBoid->size = 1.5f;
    scene.goalBoid->scale(v3(5.0f, 5.0f, 5.0f));
    scene.goalBoid->translate(v3(config.worldSize/2.0f, 0.0f, config.worldSize/2.0f - 80.0f));
    scene.boids.push_back(scene.goalBoid);
    
    return scene;
}
