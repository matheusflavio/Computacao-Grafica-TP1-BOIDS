#include "gameController.hpp"
#include "utils.hpp"
#include <iostream>
#include "GText.hpp"
#include "globals.hpp"
#include <glm/glm.hpp>
#include <cmath>
#include "vectors.hpp"
#include "GObject.hpp"
#include "boid.hpp"
#include "customObjects.hpp"
#include <set>
#include "ShadowRenderer.hpp"
#include "DynamicObstacleShadowRenderer.hpp"


GameController::GameController() {}


GLfloat lightVertices[] =
{ //     COORDINATES     //
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


float spaceShipAcceleration = 0.01;
void GameController::init(Shader *shaderProgram){
    this->shader = shaderProgram;
    
    gText = new GText();
    game::started = true;

    glfwGetWindowSize(game::window, &game::width, &game::height);
    this->camera = new Camera(game::width, game::height, glm::vec3(5.0f, 1.0f, 5.0f));

    initLight();

    // Texture
    skyTex = new Texture("textures/whitetex.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	skyTex->bindUnit(*shaderProgram, "tex0", 0);

	brickTex = new Texture("textures/stardew_valley_floor.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex->bindUnit(*shaderProgram, "tex1", 1);
    
    obstacleTex = new Texture("textures/obstacles_texture.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	obstacleTex->bindUnit(*shaderProgram, "tex2", 2);

    wallTex = new Texture("textures/portal_wall_texture2.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	wallTex->bindUnit(*shaderProgram, "tex3", 3);

    // Inicializa uniforms relacionados a fog e shadow
    this->shader->activate();
    glUniform1i(glGetUniformLocation(this->shader->id, "u_useFog"), useFog ? 1 : 0);
    glUniform1f(glGetUniformLocation(this->shader->id, "u_fogDensity"), fogDensity);
    glUniform3f(glGetUniformLocation(this->shader->id, "u_fogColor"), fogColor.x, fogColor.y, fogColor.z);
    glUniform1i(glGetUniformLocation(this->shader->id, "u_isShadow"), 0);


    // Objects
    objects.clear();
    GLfloat pi = 3.14159265f;
    GObject *wall = createWall();
    GObject *wall2 = createWall();
    GObject *wall3 = createWall();
    GObject *wall4 = createWall();
    this->plane = createPlane();
    this->sky = createPlane(v3(3, 173, 252), 1.0f);
    wall2->rotate(glm::vec3(0.0f, 270, 0.0f));

    wall4->rotate(glm::vec3(0.0f, 270, 0.0f));
    wall3->translate(glm::vec3(0.0f, 0.0f, worldSize));
    wall4->translate(glm::vec3(worldSize, 0.0f, 0.0f));
    this->sky->translate(glm::vec3(0.0f, worldHeight, 0.0f));

    this->tower = createTower(70.0f, 20.0f);
    this->tower->translate(glm::vec3((worldSize/2.0f), 0.0f, (worldSize/2.0f)));
    this->towerTop = v3(worldSize/2.0f, 70.0f, worldSize/2.0f);

    
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
    this->obstacles.push_back(tower);
    this->obstacles.push_back(t2);
    this->obstacles.push_back(t3);
    this->obstacles.push_back(t4);
    this->obstacles.push_back(t5);
    this->obstacles.push_back(t6);

    ghostBoid = new Boid(v3(100.0f, 30.0f, 100.0f), v3(1.0f, 0.0f, 0.0f));
    ghostBoid->opacity = 0.8f;
    ghostBoid->speedMultiplier = 1.06f;
    objectsPlaneText.push_back(ghostBoid);
    ghostBoid->translate(v3(30.0f, rand()%6 - 3, 30.0f));
    ghostBoid->scale(v3(0.75f, 0.75f, 0.75f));
    boids.push_back(dynamic_cast<Boid*>(ghostBoid));

    for(int i = 1; i <= 5; i++) {
        for(int j = 1; j <= 5; j++) {
            GObject *boid = new Boid(v3(20.0f * j, 30.0f, 20.0f * i));
            objectsPlaneText.push_back(boid);
            boid->translate(v3(0.0f, rand()%6 - 3, 0.0f));
            boids.push_back(dynamic_cast<Boid*>(boid));
        }
    }

    // loop boids
    for(int i = 0; i < boids.size(); i++) {
        boids[i]->originalPosition = getBoidGroupCenter() - boids[i]->getPosition();
    }

    this->goalBoid = new Boid(v3(0.0f, 30.0f, 0.0f), v3(0.0f, 0.5f, 0.0f));
    goalBoid->opacity = 0.6f;
    this->goalBoid->size = 1.5f; // hack pra arrumar batidas das asas
    goalBoid->scale(v3(5.0f, 5.0f, 5.0f));
    this->goalBoid->translate(v3(worldSize/2.0f, 0.0f, worldSize/2.0f - 80.0f));
    objectsPlaneText.push_back(goalBoid);

    objects.push_back(plane);

    walls.push_back(wall);
    walls.push_back(wall2);
    walls.push_back(wall3);
    walls.push_back(wall4);

    // loop objects
    for(int i = 0; i < objects.size(); i++) {
        objects[i]->recalculateNormals();
    }
    objectsPlaneText.push_back(this->sky);
}

void GameController::initLight() {
    this->lightShader = new Shader("shaders/light.vert", "shaders/light.frag");

    lightVAO = new VAO();
    lightVAO->bind();

    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));

    lightVAO->linkAttribute(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    lightVAO->unbind();
    lightVBO.unbind();
    lightEBO.unbind();

    v3 lightColor = v3(1.0f, 1.0f, 1.0f);
    v3 lightPos = v3(worldSize/3.0f,  600.0f, worldSize/2.0f);
    m4 lightModel = m4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    v3 pyramidPos = v3(0.0f, 0.0f, 0.0f);
    m4 pyramidModel = m4(1.0f);
    pyramidModel = glm::translate(pyramidModel, pyramidPos);

    lightShader->activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader->id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform3f(glGetUniformLocation(lightShader->id, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    this->shader->activate();
    glUniformMatrix4fv(glGetUniformLocation(this->shader->id, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
    glUniform3f(glGetUniformLocation(this->shader->id, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
    glUniform3f(glGetUniformLocation(this->shader->id, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    
    // Salva a posição e direção inicial da luz para cancelamento de preview
    savedLightPos = lightPos;
    savedLightDir = v3(0.0f, -1.0f, 0.0f);
    savedUseCustomLightDir = false;
}

void GameController::handleInput(GLuint pressedKey, GLuint pressedMouseButton, Vec2 mousePos) {

    // Use GLFW state to handle mouse hold/release for rotation reliably
    if(previewLight) {
        int leftState = glfwGetMouseButton(game::window, GLFW_MOUSE_BUTTON_LEFT);
        int rightState = glfwGetMouseButton(game::window, GLFW_MOUSE_BUTTON_RIGHT);
        // Left click cancels preview and restores previous light
        if(leftState == GLFW_PRESS) {
            previewLight = false;
            rotateLightMode = false;
            // Restore previous light state
            useCustomLightDir = savedUseCustomLightDir;
            this->shader->activate();
            glUniform3f(glGetUniformLocation(this->shader->id, "lightPos"), savedLightPos.x, savedLightPos.y, savedLightPos.z);
            glUniform1i(glGetUniformLocation(this->shader->id, "u_useLightDir"), savedUseCustomLightDir ? 1 : 0);
            if(savedUseCustomLightDir) {
                glUniform3f(glGetUniformLocation(this->shader->id, "u_lightDir"), savedLightDir.x, savedLightDir.y, savedLightDir.z);
            }
            // Update light model visual indicator
            this->lightShader->activate();
            m4 lightModel = m4(1.0f);
            lightModel = glm::translate(lightModel, savedLightPos);
            glUniformMatrix4fv(glGetUniformLocation(this->lightShader->id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
            return;
        }

        // Right click toggles/maintains rotation mode while pressed
        if(rightState == GLFW_PRESS) {
            if(!rotateLightMode) {
                rotateLightMode = true;
                // capture current cursor pos
                double cx, cy;
                glfwGetCursorPos(game::window, &cx, &cy);
                lastMouseX = cx;
                lastMouseY = cy;
            } else {
                double cx, cy;
                glfwGetCursorPos(game::window, &cx, &cy);
                double deltaX = cx - lastMouseX;
                double deltaY = cy - lastMouseY;
                // Sensibilidade: 0.5 graus por pixel
                previewLightYaw += deltaX * 0.5f;
                previewLightPitch -= deltaY * 0.5f;
                if(previewLightPitch > 89.0f) previewLightPitch = 89.0f;
                if(previewLightPitch < -89.0f) previewLightPitch = -89.0f;
                lastMouseX = cx;
                lastMouseY = cy;
            }
        } else {
            // released
            rotateLightMode = false;
        }
    }

    switch(pressedKey) {
        case GLFW_KEY_R: {
            destroy();
            init(shader);
            break;
        }
        case GLFW_KEY_C: { // Alternar preview de luz
            if(!previewLight) {
                // Entrar em preview - salva estado anterior
                previewLight = true;
                savedLightPos = previewLightPos; // será atualizado no drawElements
                savedUseCustomLightDir = useCustomLightDir;
                savedLightDir = v3(
                    glm::sin(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch)),
                    glm::sin(glm::radians(previewLightPitch)),
                    glm::cos(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch))
                );
                previewLightPos = camera->position + camera->orientation * previewLightDistance;
                // Reset rotation angles
                previewLightYaw = 0.0f;
                previewLightPitch = 0.0f;
            } else {
                // Confirmar posicionamento da luz
                previewLight = false;
                v3 lightPos = previewLightPos;
                m4 lightModel = m4(1.0f);
                lightModel = glm::translate(lightModel, lightPos);

                this->lightShader->activate();
                glUniformMatrix4fv(glGetUniformLocation(this->lightShader->id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
                this->shader->activate();
                glUniform3f(glGetUniformLocation(this->shader->id, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
                // Keep custom light direction after confirmation
                useCustomLightDir = true;
                v3 finalDir = v3(
                    glm::sin(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch)),
                    glm::sin(glm::radians(previewLightPitch)),
                    glm::cos(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch))
                );
                finalDir = glm::normalize(finalDir);
                glUniform1i(glGetUniformLocation(this->shader->id, "u_useLightDir"), 1);
                glUniform3f(glGetUniformLocation(this->shader->id, "u_lightDir"), finalDir.x, finalDir.y, finalDir.z);
                // Save as current for future cancellation
                savedLightPos = lightPos;
                savedLightDir = finalDir;
                savedUseCustomLightDir = true;
            }
            break;
        }
        case GLFW_KEY_1: { // Camera mode 1, top of the tower
            lockedPositionBehind = false;
            lockedPositionSide = false;
            v3 boidGroupCenter = getBoidGroupCenter();
            camera->position = this->towerTop + v3(0.0f, 2.0f, 0.0f);
            camera->lookAt(boidGroupCenter);
            break;
        }
        case GLFW_KEY_2: { // Camera mode 2, behind boids
            lockedPositionBehind = true;
            lockedPositionSide = false;
            camera->lookAt(getBoidGroupCenter());
            break;
        }
        case GLFW_KEY_3: { // Camera mode 3, side of the boids
            lockedPositionSide = true;
            lockedPositionBehind = false;
            lockedPositionSideCounterClockwise = false;
            break;
        }
        case GLFW_KEY_4: { // Camera mode 4, other side of the boids
            lockedPositionSide = true;
            lockedPositionBehind = false;
            lockedPositionSideCounterClockwise = true;
            break;
        }
        case GLFW_KEY_L: { // Lock or unlock fixed camera orientation pointing to boids
            this->lockedOrientation = !this->lockedOrientation;
            break;
        }
        case GLFW_KEY_0: { // Unlock camera position if fixed to move with the boids
            this->lockedPositionBehind = false;
            this->lockedPositionSide = false;
            break;
        }
        case GLFW_KEY_P: { // Pause
            paused = !paused;
            if(!paused) debugMode = false;
            break;
        }
        case GLFW_KEY_F: { // Toggle fog
            useFog = !useFog;
            // atualiza uniform no shader (será sobrescrito a cada frame em drawElements, mas atualiza já agora)
            this->shader->activate();
            glUniform1i(glGetUniformLocation(this->shader->id, "u_useFog"), useFog ? 1 : 0);
            glUniform1f(glGetUniformLocation(this->shader->id, "u_fogDensity"), fogDensity);
            glUniform3f(glGetUniformLocation(this->shader->id, "u_fogColor"), fogColor.x, fogColor.y, fogColor.z);
            break;
        }
        case GLFW_KEY_LEFT_BRACKET: { // Debug mode
            if(!debugMode) {
                paused = true;
            }else{
                allowDebugFrames++;
            }
            debugMode = true;
            break;
        }
        case GLFW_KEY_U: {
            createRandomBoid();
            break;
        }
        case GLFW_KEY_MINUS: {
            deleteRandomBoid();
            break;
        }
        case GLFW_KEY_O: { // Increase goal boid speed
            goalBoid->speed += 0.1f;
            break;
        }
        case GLFW_KEY_I: { // Decrease goal boid speed
            goalBoid->speed -= 0.1f;
            if(goalBoid->speed < 0.0f) {
                goalBoid->speed = 0.0f;
            }
            break;
        }
        case GLFW_KEY_M: { // Increase boid group speed
            for(int i = 0; i < boids.size(); i++) {
                boids[i]->speed += 0.1f;
            }
            break;
        }
        case GLFW_KEY_N: { // Decrease boid group speed
            for(int i = 0; i < boids.size(); i++) {
                boids[i]->speed -= 0.1f;
                if(boids[i]->speed < 0.0f) {
                    boids[i]->speed = 0.0f;
                }
            }
            break;
        }
    }
    if(!game::started) return;
}

// Processa scroll do mouse para ajustar distância da luz em preview
void GameController::handleScroll(double scrollDelta) {
    if(!previewLight) return; // Só afeta quando em modo preview
    
    // scroll up (positivo) = afasta (aumenta distância)
    // scroll down (negativo) = aproxima (diminui distância)
    // Sensibilidade reduzida pela metade
    previewLightDistance += scrollDelta * scrollSensitivity * 0.5f;
    
    // Limita a distância
    if(previewLightDistance < minLightDistance) previewLightDistance = minLightDistance;
    if(previewLightDistance > maxLightDistance) previewLightDistance = maxLightDistance;
    
    // Atualiza posição da prévia
    previewLightPos = camera->position + camera->orientation * previewLightDistance;
}

void GameController::resizeScreen() {
    // Recarrega o recurso de texto com as novas dimensões
    delete gText;
    gText = new GText();

    // Atualiza dimensões da câmera
    camera->width = game::width;
    camera->height = game::height;
    
    // Recomputa a matriz de projeção da câmera com a nova razão de aspecto
    // FOV: 45 graus, near: 0.1, far: 100.0
    camera->updateMatrix(45.0f, 0.1f, 100.0f);
}


float rotation = 0.0f;
double prevTime = glfwGetTime();

bool acSpVSet = false;
v3 acSpVector = v3(0.0f, 0.0f, 0.0f);
void GameController::drawElements() {

    // Atualiza câmera apenas quando NÃO estiver em preview de luz ou rotacionando a incidência
    if(!previewLight && !rotateLightMode) {
        if(this->lockedOrientation) {
            // camera->lookAt(getBoidGroupCenter());
            camera->lookAt(boids[0]->getPosition());
        }

        // Lock camera position behind boids 
        if(lockedPositionBehind || lockedPositionSide) {
            v3 boidGroupCenter = boids[0]->getPosition();
            v3 spVec = glm::normalize(boids[0]->speedVector);

            if(lockedPositionSide) {
                if(lockedPositionSideCounterClockwise) {
                    spVec = v3(-spVec.z, spVec.y, spVec.x);
                }else{
                    spVec = v3(spVec.z, spVec.y, -spVec.x);
                }
            }

            if(!acSpVSet) {
                acSpVector = spVec;
                acSpVSet = true;
            }else{
                float sp = boids[0]->speed;
                float smoothFactor = 25.0;
                float changeRatio = 1.0 - (1.0 - (1.0 / (smoothFactor * (sp + 1.0)) ));
                acSpVector = acSpVector * (1-changeRatio) + spVec * changeRatio;
            }

            v3 boidsBack = boidGroupCenter - acSpVector * 55.0f;
            camera->position = boidsBack;
        }

        // processa inputs da câmera normalmente
        camera->inputs(game::window);
    }
    camera->updateMatrix(45.0f, 0.1f, 1000.0f);

    this->shader->activate();
    glUniform3f(glGetUniformLocation(this->shader->id, "camPos"), camera->position.x, camera->position.y, camera->position.z);
    camera->matrix(this->shader, "camMatrix");

    // Atualiza parâmetros de fog no shader
    glUniform1i(glGetUniformLocation(this->shader->id, "u_useFog"), useFog ? 1 : 0);
    glUniform1f(glGetUniformLocation(this->shader->id, "u_fogDensity"), fogDensity);
    glUniform3f(glGetUniformLocation(this->shader->id, "u_fogColor"), fogColor.x, fogColor.y, fogColor.z);
    // Atualiza parâmetros de direção de luz (usado em preview e após confirmação)
    v3 currentLightDir = v3(0.0f, -1.0f, 0.0f);
    if(previewLight || useCustomLightDir) {
        currentLightDir = v3(
            glm::sin(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch)),
            glm::sin(glm::radians(previewLightPitch)),
            glm::cos(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch))
        );
        currentLightDir = glm::normalize(currentLightDir);
    }
    glUniform1i(glGetUniformLocation(this->shader->id, "u_useLightDir"), (previewLight || useCustomLightDir) ? 1 : 0);
    glUniform3f(glGetUniformLocation(this->shader->id, "u_lightDir"), currentLightDir.x, currentLightDir.y, currentLightDir.z);

    if(vao != NULL) delete vao;

    followGoal();
    checkForObstacles();
    checkForWalls();
    
    if(!paused || allowDebugFrames > 0) {
        for(int i = 0; i < boids.size(); i++) {
            boids[i]->animate();
            boids[i]->frameUpdate();
        }

        goalBoid->animate();
        goalBoid->frameUpdate();

        if(allowDebugFrames > 0) {
            printDebug();
            allowDebugFrames--;
        }
    }
    // Primeiro desenha sombras dos boids (projeção simples no plano do chão)
    // Delega para o módulo ShadowRenderer (evita responsabilidades excessivas em GameController)
    ShadowRenderer::renderGroundShadows(this->boids, this->shader);

    // Renderiza sombras dinâmicas dos obstáculos SEMPRE
    // As sombras dinâmicas são projetadas no chão (y=0) e paredes
    std::vector<GObject*> surfaces;
    if(plane != NULL) surfaces.push_back(plane);
    for(const auto& wall : walls) {
        if(wall != NULL) surfaces.push_back(wall);
    }
    
    // Renderiza sombras dinâmicas dos obstáculos com direção de luz fixa para baixo
    v3 fixedLightDir = glm::normalize(v3(0.0f, -1.0f, 0.0f)); // Luz vindo de cima
    DynamicObstacleShadowRenderer::renderDynamicShadows(obstacles, surfaces, fixedLightDir, this->shader);

    // Em seguida desenha o resto dos objetos normalmente
    drawObjects(objects, brickTex);
    drawObjects(walls, wallTex);
    drawObjects(obstacles, obstacleTex);
    drawObjects(objectsPlaneText, skyTex);

    this->lightShader->activate();
    // If we're in preview mode, ensure the light position follows the preview cube
    if(previewLight) {
        previewLightPos = camera->position + camera->orientation * previewLightDistance;
        // update main shader light position so lighting matches preview
        this->shader->activate();
        glUniform3f(glGetUniformLocation(this->shader->id, "lightPos"), previewLightPos.x, previewLightPos.y, previewLightPos.z);
        // update the small light model used by lightShader so the visual indicator follows
        this->lightShader->activate();
        m4 lightModel = m4(1.0f);
        lightModel = glm::translate(lightModel, previewLightPos);
        glUniformMatrix4fv(glGetUniformLocation(this->lightShader->id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    }
    camera->matrix(lightShader, "camMatrix");
    lightVAO->bind();
    glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
    
    
    for(int i = 0; i < flashMessages.size(); i++) {
        if(flashMessages[i].finishAt > getMillis()) {
            FlashMessage m = flashMessages[i];
            drawText(m.text, m.x, 70 * i + m.y, flashMessages[i].scale, m.rgb);
        }else{
            flashMessages.erase(flashMessages.begin()+i--);
        }
    }

    if(paused) {
        // Título
        drawText(debugMode ? "DEBUG" : "PAUSED", game::width / 2 - 80, game::height / 2 + 60, 1.0f, v3(1.0f, 1.0f, 1.0f));
        // Instruções de controle exibidas apenas enquanto pausado
        float cx = 60.0f;
        float cy = game::height / 2 + 20;
        drawText("P - Resume / Pause", cx, cy, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("F - Toggle Fog", cx, cy - 30, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("U - Add Boid", cx, cy - 60, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("- (Minus) - Remove Boid", cx, cy - 90, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("1-4 - Camera Modes", cx, cy - 120, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("L - Toggle Camera Orientation Lock", cx, cy - 150, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("C - Preview Light Position", cx, cy - 180, 0.5f, v3(0.9f, 0.9f, 0.9f));
        drawText("R - Reset Scene", cx, cy - 210, 0.5f, v3(0.9f, 0.9f, 0.9f));
    }

    // Renderiza a crosshair ciano '+' no centro da tela (SEMPRE, exceto em preview)
    if(!previewLight) {
        // Desenha '+' ciano no centro da tela, 30% do tamanho anterior
        float cx = (float)game::width / 2.0f;
        float cy = (float)game::height / 2.0f;
        drawText("+", cx - 5.0f, cy - 5.0f, 0.45f, v3(0.0f, 1.0f, 1.0f));
    }

    // Se em modo preview de luz, desenha o cubo e seta
    if(previewLight) {
        drawLightPreview();
        // Informações na tela durante preview
        drawText("LIGHT PREVIEW MODE", game::width / 2 - 150, game::height - 50, 0.6f, v3(1.0f, 1.0f, 0.0f));
        drawText("Scroll: Afasta/Aproxima | Right Click: Rotaciona", game::width / 2 - 180, game::height - 80, 0.5f, v3(1.0f, 0.8f, 0.0f));
        drawText("C: Confirmar  | Click Esq: Cancelar", game::width / 2 - 140, game::height - 110, 0.5f, v3(1.0f, 0.8f, 0.0f));
        drawText("Distance: " + std::to_string((int)previewLightDistance) + "u", game::width / 2 - 80, game::height - 140, 0.5f, v3(0.8f, 1.0f, 1.0f));
    }
}

void GameController::drawObjects(vector<GObject*> &objects, Texture* tex) {
    vao = new VAO();
    vao->bind();
    tex->bind();
    int sizeVArray = 0;
    int sizeIArray = 0;
    
    if(vArray != NULL) { free(vArray); vArray = NULL; }
    if(indices != NULL) { free(indices); indices = NULL; }
    
    long arrayPos = 0;
    long indicesPos = 0;
    int totalVertices = 0;
    int totalIndices = 0;


    for(int i = 0; i < objects.size(); i++) {
        GObject *obj = objects[i];
        if(obj->destroyed) continue;
        totalVertices += obj->vertices.size();
        totalIndices += obj->indices.size();
    }
    sizeVArray = totalVertices * 12 * sizeof(GLfloat);
    sizeIArray = totalIndices * sizeof(GLuint);

    vArray = (GLfloat*) malloc(sizeVArray);
    indices = (GLuint*) malloc(sizeIArray);

    for(int i = 0; i < objects.size(); i++) {
        objects[i]->prepare(vArray, arrayPos, indices, indicesPos);
    }


    vbo1 = new VBO(vArray, sizeVArray);
    ebo1 = new EBO(indices, sizeIArray);
    
    vao->linkAttribute(*vbo1, 0, 3, GL_FLOAT, 12 * sizeof(float), (void*)0); // pos
    vao->linkAttribute(*vbo1, 1, 4, GL_FLOAT, 12 * sizeof(float), (void*)(3 * sizeof(float))); // color
    vao->linkAttribute(*vbo1, 2, 2, GL_FLOAT, 12 * sizeof(float), (void*)(7 * sizeof(float))); // texture
    vao->linkAttribute(*vbo1, 3, 3, GL_FLOAT, 12 * sizeof(float), (void*)(9 * sizeof(float))); // normal
    
    vao->unbind();
    delete vbo1;
    delete ebo1;

    // IMPORTANTE: free vArray e indices após VBO/EBO foram deletados
    if(vArray != NULL) { free(vArray); vArray = NULL; }
    if(indices != NULL) { free(indices); indices = NULL; }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
    proj = glm::perspective(glm::radians(45.0f), ((float)game::width) / ((float)game::height), 0.1f, 100.0f);

    int modelLoc = glGetUniformLocation(this->shader->id, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    int viewLoc = glGetUniformLocation(this->shader->id, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    int projLoc = glGetUniformLocation(this->shader->id, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    GLuint uniID = glGetUniformLocation(this->shader->id, "scale");
    // Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
    glUniform1f(uniID, 0.5f);
    vao->bind();


    glDrawElements(GL_TRIANGLES, sizeIArray / sizeof(int), GL_UNSIGNED_INT, 0);
}

void GameController::drawText(string text, float x, float y, float scale, glm::vec3 colors){    
    gText->renderText(text, x, y, scale, colors);
    shader->activate();
}

void GameController::destroy() {
    // Deleta GText
    if(gText != NULL) delete gText;
    gText = NULL;

    // Free any temporary arrays
    if(vArray != NULL) { free(vArray); vArray = NULL; }
    if(indices != NULL) { free(indices); indices = NULL; }

    // Delete textures
    if(brickTex != NULL) { delete brickTex; brickTex = NULL; }
    if(skyTex != NULL) { delete skyTex; skyTex = NULL; }
    if(obstacleTex != NULL) { delete obstacleTex; obstacleTex = NULL; }
    if(wallTex != NULL) { delete wallTex; wallTex = NULL; }

    // Delete light VAO/Shader
    if(lightVAO != NULL) { delete lightVAO; lightVAO = NULL; }
    if(lightShader != NULL) { delete lightShader; lightShader = NULL; }

    // Delete cached arrow VAO/VBO/EBO
    if(arrowVAO != NULL) { delete arrowVAO; arrowVAO = NULL; }
    if(arrowVBO != NULL) { delete arrowVBO; arrowVBO = NULL; }
    if(arrowEBO != NULL) { delete arrowEBO; arrowEBO = NULL; }

    // Delete temporary main VAO if exists
    if(vao != NULL) { delete vao; vao = NULL; }

    // Collect unique pointers for deletion to avoid double-delete
    std::set<GObject*> toDelete;
    for(auto p : objects) if(p != NULL) toDelete.insert(p);
    for(auto p : objectsPlaneText) if(p != NULL) toDelete.insert(p);
    for(auto p : walls) if(p != NULL) toDelete.insert(p);
    for(auto p : obstacles) if(p != NULL) toDelete.insert(p);
    for(auto p : boids) if(p != NULL) toDelete.insert(p);
    if(plane != NULL) toDelete.insert(plane);
    if(sky != NULL) toDelete.insert(sky);
    if(tower != NULL) toDelete.insert(tower);
    if(goalBoid != NULL) toDelete.insert(goalBoid);
    if(ghostBoid != NULL) toDelete.insert(ghostBoid);

    for(auto p : toDelete) {
        try { delete p; } catch(...) {}
    }

    // Clear containers
    objects.clear();
    objectsPlaneText.clear();
    walls.clear();
    obstacles.clear();
    boids.clear();

    // Delete camera
    if(camera != NULL) { delete camera; camera = NULL; }

    flashMessages.clear();
    game::started = false;
}

GameController::~GameController() {
    destroy();
}


v3 GameController::getBoidGroupCenter() {
    // loop boids and get average pos
    if(boids.empty()) return v3(0.0f, 0.0f, 0.0f);
    
    v3 sum = v3(0.0f, 0.0f, 0.0f);
    int count = 0;
    for(int i = 0; i < boids.size(); i++) {
        if(boids[i] == NULL || boids[i]->destroyed) continue;
        sum += boids[i]->getPosition();
        count++;
    }
    if(count == 0) return v3(0.0f, 0.0f, 0.0f);
    return sum/=count;
}

bool GameController::isValidBoidPosition(v3 pos) {
    if(pos.x < 5.0f || pos.x > worldSize - 5.0f) return false;
    if(pos.z < 5.0f || pos.z > worldSize - 5.0f) return false;
    if(pos.y < 5.0f || pos.y > worldHeight - 5.0f) return false;

    // avoid boids colisions
    for(int i = 0; i < boids.size(); i++) {
        GLfloat dist = glm::distance(pos, boids[i]->getPosition());
        if(dist < 2.0f) {
            return false;
        }
    }

    return true;
}

void GameController::createRandomBoid() {
    const float horizontalRadius = 120.0f;   // distância lateral
    const float forwardBackRange = 220.0f;   // frente/atrás do centro
    const float verticalVariation = 45.0f;   // variação controlada de altura
    const int maxAttempts = 10;

    v3 center = getBoidGroupCenter();
    GObject* boid = nullptr;
    v3 pos;

    int attempts = 0;
    while (attempts < maxAttempts) {

        // deslocamento lateral (esquerda/direita)
        float dx = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * horizontalRadius;

        // deslocamento frente/atrás
        float dz = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * forwardBackRange;

        // altura controlada em torno do centro (nunca muito acima ou abaixo)
        float dy = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * verticalVariation;

        if(rand() % 2 == 0){
            pos = v3(center.x + dx,
                center.y + dy,
                center.z + dz);
        }
        else {
            pos = v3(center.x - dx,
                center.y - dy,
                center.z + dz);
        }

        // cria boid para checar posição
        boid = new Boid(pos);

        if (isValidBoidPosition(boid->getPosition()))
            break;

        delete boid;
        boid = nullptr;
        attempts++;
    }

    if (!boid) {
        // Falhou após as tentativas
        return;
    }

    // registra objeto
    objectsPlaneText.push_back(boid);

    Boid* b = dynamic_cast<Boid*>(boid);
    if (b && !boids.empty()) {
        b->syncWith(boids[0]);  // poderia sincronizar com boids mais próximos também
    }
    boids.push_back(b);

    printf("center: %f %f %f\n",
           center.x,
           center.y,
           center.z);

    printf("ghostBoid: %f %f %f\n",
           ghostBoid->getPosition().x,
           ghostBoid->getPosition().y,
           ghostBoid->getPosition().z);

    printf("[Spawn] Boid at: %f %f %f | total=%zu\n",
           pos.x, pos.y, pos.z, boids.size());
}


void GameController::deleteRandomBoid() {
    // delete random boid
    if(boids.size() > 2) {
        int index = (rand() % (boids.size()-1)) + 1;
        boids[index]->destroy();
        boids.erase(boids.begin() + index);
    }
}

bool toTheLeft(v3 a, v3 b, v3 c) {
    return ((b.x - a.x) * (c.z - a.z) - (b.z - a.z) * (c.x - a.x)) > 0;
}

void GameController::checkForWalls() {
    v3 boidsSpeed = glm::normalize(goalBoid->speedVector);
    v3 boidsPos = goalBoid->getPosition();

    bool set = false;

    for(int i = 0; i < this->walls.size(); i++) {
      
        v3 p1 = this->walls[i]->vertices[0].coords;
        v3 p2 = this->walls[i]->vertices[1].coords;
        v3 p3 = this->walls[i]->vertices[2].coords;
        // get plane based on 3 points
        v3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
        

        // get distance to closest vertice on the wall
        GLfloat distanceToWall = abs(glm::dot(boidsPos - p1, normal));
        v3 closestPoint = boidsPos - distanceToWall * normal;


        v3 newBoidPos = boidsPos + boidsSpeed;

        // if distance to a arbitrary point got smaller, this is the wall the boids are going into
        float intoWallMove = distanceToWall - abs(glm::dot(newBoidPos - p1, normal)) > 0;
        // printf("intoWallMove: %f\n", intoWallMove);

        if(intoWallMove) {
            if(distanceToWall < 20) {
                bool isToTheLeft = toTheLeft(goalBoid->getPosition(), p1 + goalBoid->speedVector, closestPoint);

                goalBoid->rotating = true;
                goalBoid->setRotatingNegative(!isToTheLeft);
                set = true;
            }
        }
    }

    // add some randomness to the rotation total movement
    if(!set) {
        goalBoid->framesToDisableRotation = rand() % (int)(((float)10)/goalBoid->speed);
    }

}

bool GameController::goingToHitTower(Boid *boidTest, GObject *tower) {

    v3 boidTestPos = boidTest->getPosition();
    float yPos = boidTestPos.y;
    boidTestPos.y = 0.0f;
    Tower *realTower = dynamic_cast<Tower*>(tower);
    float realRadius = realTower->height < yPos ? 0.0f : realTower->radius * ((realTower->height - yPos) / realTower->height);

    v3 nextboidTestPos = boidTestPos + boidTest->speedVector; nextboidTestPos.y = 0.0f;
    v3 towerPos = tower->getPosition(); towerPos.y = 0.0f;


    // get distance to tower
    GLfloat distanceToTower = glm::distance(boidTestPos, towerPos) - realRadius;
    GLfloat nextDistanceToTower = glm::distance(nextboidTestPos, towerPos) - realRadius;

    
    // if not going to hit tower stop
    if(!(distanceToTower < 15.0f)) {
        return false;
    }

    boidTest->rotating = true;
    
    v3 vec1 = boidTest->speedVector;
    v3 vec2 = towerPos - boidTestPos;

    bool isToTheLeft = glm::dot(glm::cross(vec1, vec2), glm::normalize(boidTest->getPosition())) < 0;
    if(isToTheLeft) {
        boidTest->setRotatingNegative(false);
    }else{
        boidTest->setRotatingNegative(true);
    }
    return true;
}

void GameController::checkForObstacles() {

    bool obstacleFound = false;
    for(int j = 0; j < obstacles.size(); j++){
        for(int i = 0; i < boids.size(); i++) {
            obstacleFound |= goingToHitTower(boids[i], obstacles[j]);
        }
    }

    // loop every boid (except for the first => ghost boid)
    for(int i = 1; i < boids.size(); i++) {
        if(boids[i] == NULL || boids[i]->destroyed) continue;
        // loop every boid
        for(int j = i+1; j < boids.size(); j++) {
            if(boids[j] == NULL || boids[j]->destroyed) continue;
            // if same boid, continue
            if(i == j) continue;
            // get distance to other boid
            GLfloat distance = glm::distance(boids[i]->getPosition(), boids[j]->getPosition());
            // if distance is small, try to avoid
            if(distance < 5.0f && !obstacleFound) {
                boids[i]->rotating = false;
                break;
            }
        }
    }
}

void GameController::followGoal() {
    
    for(int i = 0; i < boids.size(); i++) {
        if(boids[i] == NULL || boids[i]->destroyed) continue;
        // follow goal
        v3 goalPos = i == 0 ? goalBoid->getPosition() : ghostBoid->getPosition();
        v3 vec1 = boids[i]->speedVector;
        v3 vec2 = goalPos - boids[i]->getPosition() ;

        // get angle between boid and goal
        // get angle to rotate vec1 to vec 2, between -180 and 180
        GLfloat angle = glm::degrees(glm::acos(glm::dot(vec1, vec2) / (glm::length(vec1) * glm::length(vec2))));
        
        v3 a = vec1;
        v3 b = vec2;

        bool isToTheLeft = toTheLeft(boids[i]->getPosition(), boids[i]->getPosition() + boids[i]->speedVector, goalPos);
    
        // don't rotate if angle to goal is small (Avoid back gamera glitching)
        boids[i]->rotating = angle > 1.0;
        boids[i]->setRotatingNegative(isToTheLeft);

        if(i > 0) {
            v3 center = getBoidGroupCenter();
            float dist = glm::distance(center, boids[i]->getPosition());
            if(dist < 10.0f) {
                boids[i]->speedMultiplier = 1.0f;
            }else{
                boids[i]->speedMultiplier = 1.0f + dist/200.0f;
            }
        }
    }
}

void GameController::printDebug() {
    // PRINT COORDS of all boids, ghostboid and goalboid
    for(int i = 1; i < boids.size(); i++) {
        if(boids[i] == NULL || boids[i]->destroyed) continue;
        printf("Boid %d: %f %f %f\n", i, boids[i]->getPosition().x, boids[i]->getPosition().y, boids[i]->getPosition().z);
    }
    if(ghostBoid != NULL) printf("GhostBoid (Boids Group invisible guide): %f %f %f\n", ghostBoid->getPosition().x, ghostBoid->getPosition().y, ghostBoid->getPosition().z);
    if(goalBoid != NULL) printf("GoalBoid (Boids Group goal): %f %f %f\n", goalBoid->getPosition().x, goalBoid->getPosition().y, goalBoid->getPosition().z);
}

// Desenha sombras simples dos boids projetadas no plano y = 0
void GameController::drawShadows() {
    // Junta todos os vértices/índices dos boids
    int totalVertices = 0;
    int totalIndices = 0;
    for(int i = 0; i < boids.size(); i++) {
        if(boids[i] == NULL || boids[i]->destroyed) continue;
        totalVertices += boids[i]->vertices.size();
        totalIndices += boids[i]->indices.size();
    }
    if(totalVertices == 0 || totalIndices == 0) return;

    int sizeVArray = totalVertices * 12 * sizeof(GLfloat);
    int sizeIArray = totalIndices * sizeof(GLuint);

    GLfloat *vArray = (GLfloat*) malloc(sizeVArray);
    GLuint *iArray = (GLuint*) malloc(sizeIArray);

    long arrayPos = 0;
    long indicesPos = 0;
    int startingId = 0;
    for(int b = 0; b < boids.size(); b++) {
        GObject *obj = boids[b];
        if(obj == NULL || obj->destroyed) continue;
        // para cada vértice, projeta pra y = 0 (com um pequeno offset para evitar z-fighting)
        // compute center of object (used to bias shadow spread based on wing vertical position)
        v3 center = obj->getPosition();
        float wingStretchFactor = 0.08f; // quanto a variação em Y afeta o espalhamento do shadow
        for(int i = 0; i < obj->vertices.size(); i++) {
            v3 c = obj->vertices[i].coords;
            // projeta pra y = 0 (com um pequeno offset para evitar z-fighting)
            float shadowY = 0.05f;
            // faz as "asas" do shadow se moverem: usa deslocamento em Y do vértice relativo ao centro
            float yDelta = c.y - center.y;
            float factor = 1.0f + (yDelta * wingStretchFactor);
            // limita factor para evitar exageros
            if(factor < 0.6f) factor = 0.6f;
            if(factor > 1.4f) factor = 1.4f;
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

        for(int i = 0; i < obj->indices.size(); i++) {
            iArray[indicesPos++] = obj->indices[i] + startingId;
        }
        startingId += obj->vertices.size();
    }

    // Cria VAO/VBO/EBO temporários e desenha
    VAO shadowVAO;
    shadowVAO.bind();
    VBO shadowVBO(vArray, sizeVArray);
    EBO shadowEBO(iArray, sizeIArray);

    shadowVAO.linkAttribute(shadowVBO, 0, 3, GL_FLOAT, 12 * sizeof(float), (void*)0);
    shadowVAO.linkAttribute(shadowVBO, 1, 4, GL_FLOAT, 12 * sizeof(float), (void*)(3 * sizeof(float)));
    shadowVAO.linkAttribute(shadowVBO, 2, 2, GL_FLOAT, 12 * sizeof(float), (void*)(7 * sizeof(float)));
    shadowVAO.linkAttribute(shadowVBO, 3, 3, GL_FLOAT, 12 * sizeof(float), (void*)(9 * sizeof(float)));

    // ativa shader e informa que é uma sombra via uniform
    this->shader->activate();
    glUniform1i(glGetUniformLocation(this->shader->id, "u_isShadow"), 1);
    // mantém a matriz de câmera já configurada

    shadowVAO.bind();
    glDrawElements(GL_TRIANGLES, sizeIArray / sizeof(int), GL_UNSIGNED_INT, 0);

    // limpa (VAO/VBO/EBO saem de escopo aqui e são destruídos automaticamente)
    shadowVAO.unbind();
    shadowVBO.unbind();
    shadowEBO.unbind();
    glUniform1i(glGetUniformLocation(this->shader->id, "u_isShadow"), 0);

    // Libera memory APÓS os objetos VAO/VBO/EBO serem destruídos
    free(vArray);
    free(iArray);
}

// Desenha prévia da luz: cubo branco com seta vermelha indicando direção
void GameController::drawLightPreview() {
    if(!previewLight) return;

    this->shader->activate();
    
    // Calcula a direção da luz baseada em yaw e pitch
    v3 lightDirection = v3(
        glm::sin(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch)),
        glm::sin(glm::radians(previewLightPitch)),
        glm::cos(glm::radians(previewLightYaw)) * glm::cos(glm::radians(previewLightPitch))
    );
    lightDirection = glm::normalize(lightDirection);

    // Atualiza posição da prévia a cada frame (garante que scroll é imediatamente visível)
    previewLightPos = camera->position + camera->orientation * previewLightDistance;

    // Tamanho do cubo: tornamos o tamanho dependente da distância de forma INVERSA ao movimento
    // para que ao afastar ele pareça menor (mais perceptível o afastamento).
    float referenceDistance = 200.0f;
    float baseCubeSize = 16.0f;
    float cubeSize = baseCubeSize * (referenceDistance / previewLightDistance);
    if(cubeSize < 2.0f) cubeSize = 2.0f;
    float half = cubeSize / 2.0f;

    // Vértices do cubo
    GLfloat cubeVertices[] = {
        // Positions           // Colors (branco com opacidade)
        // Bottom face
        -half, -half, -half,  1.0f, 1.0f, 1.0f,
        half, -half, -half,   1.0f, 1.0f, 1.0f,
        half, half, -half,    1.0f, 1.0f, 1.0f,
        -half, half, -half,   1.0f, 1.0f, 1.0f,
        // Top face
        -half, -half, half,   1.0f, 1.0f, 1.0f,
        half, -half, half,    1.0f, 1.0f, 1.0f,
        half, half, half,     1.0f, 1.0f, 1.0f,
        -half, half, half,    1.0f, 1.0f, 1.0f
    };

    // Índices do cubo (apenas arestas - GL_LINES)
    GLuint cubeIndices[] = {
        // Bottom face
        0, 1, 1, 2, 2, 3, 3, 0,
        // Top face
        4, 5, 5, 6, 6, 7, 7, 4,
        // Conecta top e bottom
        0, 4, 1, 5, 2, 6, 3, 7
    };

    // Cria VAO/VBO temporários
    VAO previewVAO;
    previewVAO.bind();
    
    VBO previewVBO(cubeVertices, sizeof(cubeVertices));
    EBO previewEBO(cubeIndices, sizeof(cubeIndices));
    
    previewVAO.linkAttribute(previewVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    previewVAO.linkAttribute(previewVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Desenha o cubo
    glm::mat4 model = glm::translate(glm::mat4(1.0f), previewLightPos);
    glUniformMatrix4fv(glGetUniformLocation(this->shader->id, "model"), 1, GL_FALSE, glm::value_ptr(model));
    
    glLineWidth(2.0f);
    glDrawElements(GL_LINES, sizeof(cubeIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    glLineWidth(1.0f);

    previewVAO.unbind();
    previewVBO.unbind();
    previewEBO.unbind();

    // Desenha uma seta sólida: cilindro (haste) + cone (ponta), alinhada com lightDirection
    // Parâmetros do mesh
    int segments = 18;
    float totalLength = cubeSize * 1.5f;
    float cylLength = totalLength * 0.7f;
    float coneLength = totalLength - cylLength;
    float cylRadius = cubeSize * 0.08f;
    float coneBaseRadius = cubeSize * 0.30f; // base do cone maior que o cilindro

    // Se o tamanho do cubo mudou ou é a primeira vez, regenera o cache
    if(cachedArrowCubeSize != cubeSize) {
        // Deleta cache anterior se existir
        if(arrowVAO != NULL) delete arrowVAO;
        if(arrowVBO != NULL) delete arrowVBO;
        if(arrowEBO != NULL) delete arrowEBO;

        // Gera vértices e índices dinamicamente
        std::vector<GLfloat> verts;
        std::vector<GLuint> inds;

        // Cylinder vertices (bottom ring uses cylRadius, top ring uses coneBaseRadius so the cone base is wider)
        // aligned along +Z by default
        for(int i = 0; i < segments; i++) {
            float theta = (2.0f * glm::pi<float>() * i) / segments;
            float xB = cos(theta) * cylRadius;
            float yB = sin(theta) * cylRadius;
            float xT = cos(theta) * coneBaseRadius;
            float yT = sin(theta) * coneBaseRadius;
            // bottom ring (z = 0) - cylinder base
            verts.push_back(xB); verts.push_back(yB); verts.push_back(0.0f);
            verts.push_back(1.0f); verts.push_back(0.0f); verts.push_back(0.0f); // color red
            // normal (approx)
            verts.push_back(xB); verts.push_back(yB); verts.push_back(0.0f);
            // top ring (z = cylLength) - wider ring (cone base)
            verts.push_back(xT); verts.push_back(yT); verts.push_back(cylLength);
            verts.push_back(1.0f); verts.push_back(0.0f); verts.push_back(0.0f);
            verts.push_back(xT); verts.push_back(yT); verts.push_back(0.0f);
        }
        // Tip vertex
        int tipIndex = verts.size() / 9; // each vertex has 9 floats (pos(3)+color(3)+normal(3))
        verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(totalLength);
        verts.push_back(1.0f); verts.push_back(0.0f); verts.push_back(0.0f);
        verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(1.0f);

        // Indices for cylinder sides
        for(int i = 0; i < segments; i++) {
            int b0 = i * 2;
            int t0 = i * 2 + 1;
            int b1 = ((i+1) % segments) * 2;
            int t1 = ((i+1) % segments) * 2 + 1;
            // convert to actual vertex indices (each stored pair is two vertices per segment)
            inds.push_back(b0);
            inds.push_back(t0);
            inds.push_back(t1);
            inds.push_back(b0);
            inds.push_back(t1);
            inds.push_back(b1);
        }

        // Indices for cone (connect top ring to tip)
        for(int i = 0; i < segments; i++) {
            int topIdx = i * 2 + 1;
            int nextTop = ((i+1) % segments) * 2 + 1;
            inds.push_back(topIdx);
            inds.push_back(nextTop);
            inds.push_back(tipIndex);
        }

        // Create cached VAO/VBO/EBO
        arrowVAO = new VAO();
        arrowVAO->bind();
        arrowVBO = new VBO(verts.data(), verts.size() * sizeof(GLfloat));
        arrowEBO = new EBO(inds.data(), inds.size() * sizeof(GLuint));
        // Each vertex: pos(3), color(3), normal(3) => stride 9 floats
        arrowVAO->linkAttribute(*arrowVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void*)0);
        arrowVAO->linkAttribute(*arrowVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        arrowVAO->linkAttribute(*arrowVBO, 3, 3, GL_FLOAT, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        
        arrowIndexCount = inds.size();
        cachedArrowCubeSize = cubeSize;
    }

    // Calcula rotação para alinhar +Z com lightDirection
    v3 defaultDir = v3(0.0f, 0.0f, 1.0f);
    float dot = glm::clamp(glm::dot(defaultDir, lightDirection), -1.0f, 1.0f);
    glm::vec3 rotAxis;
    float angle = 0.0f;
    if(dot < 0.9999f) {
        rotAxis = glm::cross(defaultDir, lightDirection);
        angle = acos(dot);
    } else {
        rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        angle = 0.0f;
    }

    glm::mat4 rot = glm::mat4(1.0f);
    if(angle != 0.0f) rot = glm::rotate(glm::mat4(1.0f), angle, rotAxis);
    glm::mat4 modelArrow = glm::translate(glm::mat4(1.0f), previewLightPos) * rot;
    glUniformMatrix4fv(glGetUniformLocation(this->shader->id, "model"), 1, GL_FALSE, glm::value_ptr(modelArrow));

    // Desenha com o cache
    arrowVAO->bind();
    glEnable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, arrowIndexCount, GL_UNSIGNED_INT, 0);
    glDisable(GL_DEPTH_TEST);
    arrowVAO->unbind();
}