#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include <glm/glm.hpp>
#include "vectors.hpp"

// InputHandler: responsável por processar entrada do teclado e mouse.
// Encapsula a lógica de detecção de keys/buttons e cálculo de estados.
class InputHandler {
public:
    // Estado do mouse durante preview da luz
    struct MouseState {
        double x;
        double y;
        bool rightButtonPressed;
    };

    // Estado geral da entrada
    struct InputState {
        bool togglePause;
        bool toggleFog;
        bool startLightPreview;
        bool cancelLightPreview;
        bool confirmLightPreview;
        bool scrollUp;
        bool scrollDown;
        float scrollDelta; // quantidade de scroll acumulado
        bool resetScene;
        MouseState mouse;
    };

    InputHandler();

    // Processa entrada de teclado
    void handleKeyInput(GLuint pressedKey, MouseState mouseState);
    
    // Processa entrada de mouse (button)
    void handleMouseInput(GLuint pressedMouseButton, MouseState mouseState);
    
    // Processa scroll
    void handleScrollInput(double delta);
    
    // Retorna e limpa o estado acumulado de entrada
    InputState getAndClearState();
    
    // Query do estado atual de mouse durante preview
    MouseState getCurrentMouseState() const { return currentMouse; }

private:
    InputState accumulatedState;
    MouseState currentMouse;
    bool rightMouseButtonHeld;
};

#endif
