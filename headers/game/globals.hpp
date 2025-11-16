#ifndef globals_hpp
#define globals_hpp

#include <stdio.h>
#include <GLFW/glfw3.h>

// Nível de debug (DEBUG_ALL = 1 para habilitado)
#define DEBUG_ALL 1

// Namespace com variáveis globais do jogo
namespace game {
    extern GLFWwindow *window;  // Janela do GLFW
    extern int width, height;    // Dimensões da janela
    extern int shootingInterval; // Intervalo entre disparos
    extern bool colisionsEnabled; // Se colisões estão habilitadas
    extern bool started;         // Se o jogo começou
    extern long pausedTime;      // Tempo total pausado
    extern long pausedAt;        // Quando foi pausado
    extern bool paused;          // Se está pausado
}

extern int debugLevel;           // Nível de debug global
extern long framesSinceUpdate;   // Frames desde última atualização

#endif