#include "boid.hpp"

// Anima as asas do boid
void Boid::animate() {
    float animationLimit = size * 2.0;
    
    // Inicializa a animação na primeira vez
    if(!animationStarted) {
        yCoord = vertices[leftWingId].coords.y;
        animationStarted = true;
    }

    // Faz as asas baterem para cima e para baixo
    if(goingUp) {
        vertices[leftWingId].coords.y += size / 3.0;
        vertices[rightWingId].coords.y += size / 3.0;
        if(vertices[leftWingId].coords.y >= yCoord + animationLimit) {
            goingUp = false;
        }
    } else {
        vertices[leftWingId].coords.y -= size / 3.0;
        vertices[rightWingId].coords.y -= size / 3.0;
        if(vertices[leftWingId].coords.y <= yCoord - animationLimit) {
            goingUp = true;
        }
    }
}

// Atualiza o estado do boid a cada frame
void Boid::frameUpdate() {
    // Chama o frameUpdate da classe pai
    GObject::frameUpdate();

    // Calcula banking (roll) baseado em mudança de velocidade/aceleração lateral
    // Executa sempre (mesmo quando não está rotacionando em yaw)
    calculateBankingAngle();

    // Se não está rotacionando em yaw, apenas prossegue com banking já aplicado acima
    if(!rotating) {
        return;
    }

    // Controla duração da rotação
    if(framesToDisableRotation > -1) {
        framesToDisableRotation--;
        if(framesToDisableRotation == -1) {
            rotating = false;
            return;
        }
    }

    // Calcula velocidade de rotação
    GLfloat rotateSpeed = 10.0f * speed * speedMultiplier;
    this->rotateBoid(rotatingNeg ? -rotateSpeed : rotateSpeed);
}

// Rotaciona o boid em graus e atualiza seu vetor de velocidade
void Boid::rotateBoid(float degree) {
    // Rotaciona o objeto
    this->rotate(v3(0, degree, 0));
    
    // Rotaciona o vetor de velocidade acompanhando a rotação
    this->speedVector = glm::rotateY(this->speedVector, glm::radians(degree));
    rotated += degree;
}

// Define se está rotacionando negativamente
void Boid::setRotatingNegative(bool flag) {
    rotatingNeg = flag;
}

// Retorna a posição do boid (vértice do tronco para não balançar com as asas)
v3 Boid::getPosition() {
    return this->vertices[5].coords;
}

// Sincroniza com outro boid (cópia rotação, velocidade)
void Boid::syncWith(Boid *boid) {
    this->rotating = boid->rotating;
    this->rotateBoid(boid->rotated);
    this->speed = boid->speed;
}

// Construtores
Boid::Boid(v3 translation) : Boid(translation, v3(0.0f, 0.0f, 0.0f)) {}

Boid::Boid(v3 translation, v3 color) {
    // Dimensões do boid
    float heightWings = 1.5f;
    float headLength = 2.0f;
    float backWidth = 2.0f;
    float tailLength = 1.7f;

    // Define cores
    bool userDefaultColors = false;
    if(color.x == 0.0f && color.y == 0.0f && color.z == 0.0f) {
        userDefaultColors = true;
    }

    v3 wingsColor = userDefaultColors ? v3(.419f, 0.519f, 0.919f) : (color + v3(.419f, 0.519f, 0.919f)) / 2.0f;
    v3 trunkColor = userDefaultColors ? v3(0.2f, 0.2f, 0.2f) : (color + v3(0.2f, 0.2f, 0.2f)) / 2.0f;
    v3 backColor = userDefaultColors ? v3(0.04f, 0.04f, 0.04f) : (color + v3(0.04f, 0.04f, 0.04f)) / 2.0f;

    // ===== ASA ESQUERDA =====
    this->leftWingId = this->addVertice(GVertice(v3(0.0f, 0.0f, 0.0f), wingsColor, v2(0.0f, 0.0f)));
    int v2i = this->addVertice(GVertice(v3(3.0f, -heightWings / 2, 1.5f), trunkColor, v2(5.0f, 0.0f)));
    int v3i = this->addVertice(GVertice(v3(2.6f, -heightWings / 2, -1.5f), trunkColor, v2(5.0f, 0.0f)));
    int v4i = this->addVertice(GVertice(v3(3.0f, heightWings / 2, 1.5f), trunkColor, v2(5.0f, 0.0f)));
    int v5i = this->addVertice(GVertice(v3(2.6f, heightWings / 2, -1.5f), trunkColor, v2(5.0f, 0.0f)));
    
    this->addTriangle(this->leftWingId, v2i, v3i);
    this->addTriangle(this->leftWingId, v4i, v5i);
    this->addTriangle(this->leftWingId, v2i, v4i);
    this->addTriangle(this->leftWingId, v3i, v5i);

    // ===== TRONCO =====
    int trli = this->addVertice(GVertice(v3(3.4f, -heightWings / 2, -1.5f), trunkColor, v2(5.0f, 0.0f)));
    int trhi = this->addVertice(GVertice(v3(3.4f, heightWings / 2, -1.5f), trunkColor, v2(5.0f, 0.0f)));
    this->addTriangle(v2i, v3i, trli);
    this->addTriangle(v4i, v5i, trhi);

    // ===== CABEÇA =====
    int headI = this->addVertice(GVertice(v3(3.0f, 0.0f, -1.5 - headLength), v3(0.52f, 0.12f, 0.2f), v2(5.0f, 0.0f)));
    this->addTriangle(v5i, trhi, headI);
    this->addTriangle(v3i, trli, headI);
    this->addTriangle(v3i, v5i, headI);
    this->addTriangle(trli, trhi, headI);

    // ===== ASA DIREITA =====
    this->rightWingId = this->addVertice(GVertice(v3(6.0f, 0.0f, 0.0f), wingsColor, v2(0.0f, 0.0f)));
    this->addTriangle(v2i, this->rightWingId, trli);
    this->addTriangle(v4i, this->rightWingId, trhi);
    this->addTriangle(v2i, this->rightWingId, v4i);
    this->addTriangle(trli, this->rightWingId, trhi);

    // ===== CAUDA =====
    int tailL = this->addVertice(GVertice(v3(3.0f - backWidth / 2, 0, 1.5f + tailLength), backColor, v2(0.0f, 0.0f)));
    int tailR = this->addVertice(GVertice(v3(3.0f + backWidth / 2, 0, 1.5f + tailLength), backColor, v2(0.0f, 0.0f)));
    this->addTriangle(tailL, tailR, v4i);
    this->addTriangle(tailL, tailR, v2i);
    this->addTriangle(tailR, v2i, v4i);
    this->addTriangle(tailL, v2i, v4i);

    // ===== TRANSFORMAÇÕES FINAIS =====
    this->rotate(v3(0.0f, 180.0f, 0.0f));
    this->translate(translation);
    this->scale(v3(size, size, size));

    // Gera normais automaticamente
    this->generateNormals = true;

    // Define velocidade inicial
    this->speedVector = v3(0.00f, 0.00f, 1.0f);
    this->speed = 0.1f;
}

// Calcula e aplica um pequeno ângulo de roll (em torno do eixo Z local) para simular banking
void Boid::calculateBankingAngle() {
    v3 currentPos = this->getPosition();

    // Inicialização do histórico
    if(banking_frameCounter == 0) {
        banking_prevPos = currentPos;
        banking_lastVel = this->speedVector;
        banking_frameCounter++;
        return;
    }

    // velocidade estimada (posição atual - posição anterior)
    v3 vel = currentPos - banking_prevPos;
    // aceleração aproximada (variação de velocidade)
    v3 accel = vel - banking_lastVel;

    float accelMag = glm::length(accel);
    float speedMag = glm::length(this->speedVector);

    float targetRollRad = 0.0f;
    if(speedMag > 0.01f) {
        // curvatura aproximada = atan(|a| / |v|) ; reduzimos a magnitude para efeito visual
        targetRollRad = atan(accelMag / speedMag) * 0.25f;

        // Determina o sinal do roll usando o produto vetorial entre forward e aceleração lateral
        v3 forward = glm::normalize(this->speedVector);
        float sign = 1.0f;
        if(glm::length(accel) > 0.0001f) {
            v3 lateral = accel;
            float crossY = glm::dot(glm::cross(forward, lateral), v3(0.0f, 1.0f, 0.0f));
            sign = crossY >= 0.0f ? 1.0f : -1.0f;
        }
        targetRollRad *= sign;
    }

    float targetDeg = glm::degrees(targetRollRad);
    // limita o ângulo para evitar rotações exageradas
    if(targetDeg > 25.0f) targetDeg = 25.0f;
    if(targetDeg < -25.0f) targetDeg = -25.0f;

    this->targetRollDeg = targetDeg;

    // interpolação suave
    float deltaDeg = (this->targetRollDeg - this->currentRollDeg) * 0.15f;

    if(fabs(deltaDeg) > 0.0005f) {
        // Aplica rotação em Z (roll) no modelo geométrico
        this->rotate(v3(0.0f, 0.0f, deltaDeg));
        this->currentRollDeg += deltaDeg;
    }

    // atualiza histórico
    banking_lastVel = vel;
    banking_prevPos = currentPos;
    banking_frameCounter++;
}