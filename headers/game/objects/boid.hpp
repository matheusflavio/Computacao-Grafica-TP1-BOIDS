
#ifndef boid_hpp
#define boid_hpp

#include "GObject.hpp"
#include <glm/glm.hpp>
#include "utils.hpp"

typedef glm::vec3 v3;
typedef glm::vec2 v2;

class Boid : public GObject {
    public:
        void animate();
        void frameUpdate();
        Boid(v3 translation);
        Boid(v3 translation, v3 color);

        // Banking: roll (inclinação) visual para curvas
        // Implementação simplificada: calcula curvatura aproximada e aplica pequena rotação em Z
        void calculateBankingAngle();
        float getCurrentRollDeg();

        bool rotating = 0.0f;
        vector<GLfloat> rotatingQueue;

        v3 getPosition();

        void rotateBoid(float degree);
        void syncWith(Boid *boid);

        float rotated = 0.0f;

        int framesToDisableRotation = -1;

        v3 originalPosition;

        float size = 0.3f;

        void setRotatingNegative(bool flag);

    // Banking state (degrees)
    float currentRollDeg = 0.0f; // rotação em Z aplicada ao modelo (graus)
    float targetRollDeg = 0.0f;

    private:
        bool rotatingNeg = false;
        int leftWingId;
        int rightWingId;

        bool goingUp = true;

        bool animationStarted = false;
        float yCoord;
        // Banking internals
        v3 banking_prevPos = v3(0.0f);
        v3 banking_lastVel = v3(0.0f);
        int banking_frameCounter = 0;


};

#endif