#ifndef gameController_hpp
#define gameController_hpp

#include <stdio.h>
#include <vector>
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "GText.hpp"
#include "vectors.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "GObject.hpp"
#include "boid.hpp"


using namespace std;


struct FlashMessage {
    float x, y;
    string text;
    glm::vec3 rgb;
    long finishAt;
    float scale;
    
    FlashMessage(string text, float x, float y, glm::vec3 rgb, long finishAt) : text(text), x(x), y(y), rgb(rgb), finishAt(finishAt), scale(0.6) {}
    
    FlashMessage(string text, float x, float y, glm::vec3 rgb, long finishAt, float scale) : text(text), x(x), y(y), rgb(rgb), finishAt(finishAt), scale(scale) {}
};

class GameController {
public:
    void drawElements();
    void drawObjects(vector<GObject*> &objects, Texture *tex);
    void detectColisions();
    void handleInput(GLuint pressedKey, GLuint pressedMouseButton, Vec2 mousePos);
    void handleScroll(double scrollDelta);
    void drawLightPreview();
    void drawText(string text, float x, float y, float scale, glm::vec3 colors);
    void destroy();
    void resizeScreen();
    void drawShadows();
    
    ~GameController();
    
    bool playerAlive = false;
    
    void init(Shader *shaderProgram);
    void initLight();
    
    GameController();
    
private:
    int allowDebugFrames = 0;
    bool debugMode = false;

    float worldSize = 360.0f;
    float worldHeight = 200.0f;

    VAO* vao = NULL;
    VBO* vbo1;
    EBO* ebo1;
    GLfloat* vArray = NULL;
    GLuint* indices = NULL;
    GText *gText;
    
    vector<FlashMessage> flashMessages;
    Shader *shader;

    VAO *lightVAO;
    Shader *lightShader;

    Texture *brickTex;
    Texture *skyTex;
    Texture *obstacleTex;
    Texture *wallTex;
    Camera *camera;

    vector<GObject*> objects;
    vector<GObject*> objectsPlaneText;
    vector<Boid*> boids;
    vector<GObject*> obstacles;

    Boid* goalBoid;
    Boid* ghostBoid;

    vector<GObject*> walls;

    GObject* plane;
    GObject* sky;
    GObject* tower;

    v3 towerTop;
    
    bool goalBoidRotating = false;
    long lastAttack = 0;
    bool lockedOrientation = true;
    bool lockedPositionBehind = false;
    bool lockedPositionSide = false;
    bool lockedPositionSideCounterClockwise = false;
    bool paused = false;
    bool useFog = false; // controla se a neblina está ativa
    float fogDensity = 0.02f;
    glm::vec3 fogColor = glm::vec3(0.03f, 0.06f, 0.08f);
    // Preview de posição da luz antes de confirmar
    bool previewLight = false;
    v3 previewLightPos = v3(0.0f, 0.0f, 0.0f);
    float previewLightDistance = 200.0f; // distância padrão para preview
    float minLightDistance = 50.0f;
    float maxLightDistance = 500.0f;
    float scrollSensitivity = 10.0f; // quanto cada scroll altera a distância
    // Rotação da direção da luz durante preview (ângulos em graus)
    float previewLightYaw = 0.0f;   // rotação em Y
    float previewLightPitch = 0.0f; // rotação em X
    bool rotateLightMode = false;   // está em modo de rotação de luz?
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    // Quando confirmado, usamos direção customizada também
    bool useCustomLightDir = false;
    // Armazena estado anterior da luz para cancelar preview
    v3 savedLightPos = v3(0.0f, 0.0f, 0.0f);
    v3 savedLightDir = v3(0.0f, -1.0f, 0.0f);
    bool savedUseCustomLightDir = false;
    // Cache de arrow VAO/VBO/EBO para evitar regeneração a cada frame
    VAO* arrowVAO = NULL;
    VBO* arrowVBO = NULL;
    EBO* arrowEBO = NULL;
    int arrowIndexCount = 0;
    float cachedArrowCubeSize = -1.0f;

    bool isValidBoidPosition(v3 pos);
    void checkForWalls();
    void checkForObstacles();
    void createRandomBoid();
    void deleteRandomBoid();
    void followGoal();
    bool goingToHitTower(Boid *boid, GObject *obstacle);
    void printDebug();
    v3 getBoidGroupCenter();
};

#endif /* gameController_hpp */
