#include "InputHandler.hpp"
#include "globals.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

InputHandler::InputHandler() 
    : currentMouse{0.0, 0.0, false}, rightMouseButtonHeld(false) {
    std::memset(&accumulatedState, 0, sizeof(InputState));
}

void InputHandler::handleKeyInput(GLuint pressedKey, MouseState mouseState) {
    currentMouse = mouseState;
    
    switch(pressedKey) {
        case GLFW_KEY_P:
            accumulatedState.togglePause = true;
            break;
        case GLFW_KEY_F:
            accumulatedState.toggleFog = true;
            break;
        case GLFW_KEY_C:
            // C pode ser preview (início) ou confirmação
            accumulatedState.startLightPreview = true;
            accumulatedState.confirmLightPreview = true;
            break;
        case GLFW_KEY_Z:
            accumulatedState.cancelLightPreview = true;
            break;
        case GLFW_KEY_R:
            accumulatedState.resetScene = true;
            break;
    }
}

void InputHandler::handleMouseInput(GLuint pressedMouseButton, MouseState mouseState) {
    currentMouse = mouseState;
    
    if(pressedMouseButton == GLFW_MOUSE_BUTTON_RIGHT) {
        rightMouseButtonHeld = true;
        currentMouse.rightButtonPressed = true;
    }
}

void InputHandler::handleScrollInput(double delta) {
    accumulatedState.scrollDelta += (float)delta;
    if(delta > 0.0) {
        accumulatedState.scrollUp = true;
    } else if(delta < 0.0) {
        accumulatedState.scrollDown = true;
    }
}

InputHandler::InputState InputHandler::getAndClearState() {
    InputState result = accumulatedState;
    result.mouse = currentMouse;
    
    // Limpa flags de eventos (mas mantém estado persistente como mouse)
    std::memset(&accumulatedState, 0, sizeof(InputState));
    
    return result;
}
