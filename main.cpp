#include <iostream>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include "utils.hpp"
#include "vectors.hpp"
#include "shaderClass.hpp"
#include <stb/stb_image.h>
#include <map>
#include "globals.hpp"
#include "gameController.hpp"
#include "GText.hpp"
#include "texture.hpp"

using namespace std;

// Controlador principal da aplicação
GameController gameController;

// Variáveis globais para gerenciar entrada do usuário
static GLuint pressedKey;
static GLuint pressedMouseButton = -1;
static map<GLuint, bool> keyIsPressed;
static bool holdingKey;

// Razão de aspecto da janela
float aspectRatio;

// Callback para eventos de teclado
static void keyCallback(GLFWwindow* windowGame, int key, int scancode, int action, int mods){
    switch (key) {
        case GLFW_KEY_ESCAPE:
        case GLFW_PRESS:
            glfwSetWindowShouldClose(windowGame, GLFW_TRUE);
            break;
        default:
            if(pressedKey == key && action == GLFW_RELEASE) {
                pressedKey = (GLuint) NULL;
                keyIsPressed[pressedKey] = false;
            }else if(action == GLFW_PRESS || action == GLFW_REPEAT){
                if(action == GLFW_PRESS) {
                    pressedKey = key;
                }
                keyIsPressed[pressedKey] = true;
                holdingKey = action == GLFW_REPEAT;
            }
            break;
    }
}

// Callback para eventos de botão do mouse
void mouseButtonCallback(GLFWwindow* windowGame, int button, int action, int mods){
    if (action == GLFW_PRESS) {
        pressedMouseButton = button;
    }else{
        pressedMouseButton = -1;
    }
}

// Callback para redimensionamento da janela
void windowSizeCallback(GLFWwindow* window, int width, int height){
    // Atualiza as dimensões globais da janela
    game::width = width;
    game::height = height;
    
    // Atualiza a razão de aspecto
    if(height > 0) {
        aspectRatio = (float)width / (float)height;
    }
    
    // Configura o viewport do OpenGL para o novo tamanho
    glViewport(0, 0, width, height);
    
    // Recarrega os recursos dependentes de dimensões
    gameController.resizeScreen();
}

// Callback para movimento da janela
void windowPosCallback(GLFWwindow* window, int xpos, int ypos){
    // Vazio, mas mantido para possível uso futuro
}

// Posição do mouse
static Vec2 mousePos;

// Scroll do mouse (acumulado)
static double scrollY = 0.0;

// Callback para movimento do mouse
static void cursorPositionCallback(GLFWwindow* window, double x, double y){
    mousePos = Vec2(x, y);
}

// Callback para scroll do mouse
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    scrollY = yoffset; // yoffset > 0 = scroll up, < 0 = scroll down
}

int main(void){
    // ===== INICIALIZAÇÃO DO GLFW =====
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Obtém informações do monitor primário
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GL_SCISSOR_TEST, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    // ===== CRIAÇÃO DA JANELA =====
    game::window = glfwCreateWindow(1366, 708, "BOIDS - TP1", NULL, NULL);
    if(game::window == NULL) {
        cout << "Erro: Falha ao criar janela" << endl;
        glfwTerminate();
        return 0;
    }
    
    // ===== CONFIGURAÇÃO DO CONTEXTO OPENGL =====
    glfwMakeContextCurrent(game::window);
    glfwSwapInterval(1); // Habilita sincronização com V-Sync para limitar FPS
    gladLoadGL();
    
    // ===== CALLBACKS =====
    glfwSetKeyCallback(game::window, keyCallback);
    glfwSetMouseButtonCallback(game::window, mouseButtonCallback);
    glfwSetCursorPosCallback(game::window, cursorPositionCallback);
    glfwSetScrollCallback(game::window, scrollCallback);
    glfwSetWindowSizeCallback(game::window, windowSizeCallback);
    glfwSetWindowPosCallback(game::window, windowPosCallback);
    
    // ===== OBTENÇÃO DAS DIMENSÕES DA JANELA =====
    glfwGetWindowSize(game::window, &game::width, &game::height);
    cout << "Altura: " << game::height << ", Largura: " << game::width << endl;
    aspectRatio = (float)game::width / (float)game::height;
    
    // ===== CONFIGURAÇÃO DO OPENGL =====
    // Habilita blending para renderização de texto
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ===== INICIALIZAÇÃO DO SHADER E CONTROLADOR =====
    Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
    gameController.init(&shaderProgram);
    
    // ===== VARIÁVEIS DE CONTROLE DE FPS =====
    vector<int> fpsValues = {60, 60, 60};
    long lastRealTime = getRealMillis();
    long lastGameTime = getMillis();
    long lastFpsUpdate = getMillis();
    string fpsString;
    
    // Tempo necessário para cada frame em millisegundos (1000ms / 60 FPS ≈ 16.67ms)
    const double FRAME_TIME_MS = 1000.0 / 60.0;

    // ===== LOOP PRINCIPAL =====
    while (!glfwWindowShouldClose(game::window)) {
        
        long gameTimeElapsed = getMillis() - lastGameTime;
        
        // Em modo debug, garante um mínimo de tempo entre frames
        if(gameTimeElapsed == 0) gameTimeElapsed = 10;
        
        long realTimeElapsed = getRealMillis() - lastRealTime;
        
        // Aguarda tempo suficiente para manter 60 FPS (16.67ms por frame)
        if(realTimeElapsed >= FRAME_TIME_MS) {
            glEnable(GL_DEPTH_TEST);

            // Atualiza tempos para o próximo frame
            lastGameTime = getMillis();
            framesSinceUpdate = gameTimeElapsed;
            lastRealTime = getRealMillis();
            
            // Processa eventos de entrada
            glfwPollEvents();
            
            // ===== CÁLCULO DO FPS =====
            // Mantém um histórico dos últimos 3 frames para calcular média
            fpsValues.erase(fpsValues.begin() + 0);
            fpsValues.push_back(1000 / gameTimeElapsed);
            int currentFps = (fpsValues[0] + fpsValues[1] + fpsValues[2]) / 3;
            
            // ===== RENDERIZAÇÃO =====
            glClearColor(0.03f, 0.06f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Processa entrada do usuário
            gameController.handleInput(pressedKey, pressedMouseButton, mousePos);
            gameController.handleScroll(scrollY);
            scrollY = 0.0; // reseta após processar
            pressedKey = -1;
            pressedMouseButton = -1; // Reseta para evitar ações duplicadas

            // Renderiza elementos do jogo
            gameController.drawElements();
            
            // Atualiza FPS display a cada 100ms
            if(getMillis() - lastFpsUpdate > 100) {
                fpsString = "FPS " + to_string(currentFps);
                lastFpsUpdate = getMillis();
            }
            
            // Renderiza texto do FPS com cor baseada no desempenho
            glm::vec3 fpsColor = currentFps < 60 ? glm::vec3(0.8, 0.1f, 0.1f) : glm::vec3(0.1, 0.7f, 0.1f);
            gameController.drawText(fpsString, game::width - 70, game::height - 30, 0.3f, fpsColor);
            
            glFlush();
        }
    }
    
    // ===== LIMPEZA E ENCERRAMENTO =====
    gameController.destroy();
    glfwDestroyWindow(game::window);
    glfwTerminate();
    return 0;
}
