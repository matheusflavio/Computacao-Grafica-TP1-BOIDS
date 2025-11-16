#ifndef LIGHT_PREVIEW_CONTROLLER_HPP
#define LIGHT_PREVIEW_CONTROLLER_HPP

#include <glm/glm.hpp>
#include "vectors.hpp"
#include "shaderClass.hpp"
#include "camera.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"

// LightPreviewController: encapsula lógica de preview interativo da fonte de luz.
// Gerencia posição, orientação, distância e renderização do preview.
class LightPreviewController {
public:
    struct PreviewState {
        bool isActive;
        v3 lightPosition;
        float distance;
        float yaw;    // rotação em Y (graus)
        float pitch;  // rotação em X (graus)
        v3 direction; // direção calculada
    };

    LightPreviewController();
    ~LightPreviewController();

    // Inicializa o preview com posição/orientação da câmera
    void startPreview(const v3& cameraPos, const v3& cameraOrientation);
    
    // Atualiza distância (via scroll)
    void updateDistance(float delta);
    
    // Atualiza orientação (yaw/pitch via mouse)
    void updateOrientation(float deltaYaw, float deltaPitch);
    
    // Atualiza posição calculada baseada em câmera
    void updatePosition(const v3& cameraPos, const v3& cameraOrientation);
    
    // Obtém estado atual do preview
    PreviewState getState() const;
    
    // Desenha visualização do preview (cubo + seta)
    void render(Shader* shader, const glm::mat4& view, const glm::mat4& projection);
    
    // Cancela o preview e restaura estado anterior
    void cancel();
    
    // Confirma o preview (salva como estado atual)
    void confirm();
    
    // Renderiza texto na tela informando controles
    void drawControlsText();

private:
    PreviewState currentState;
    PreviewState savedState;
    
    // Cache de arrow (seta indicadora)
    VAO* arrowVAO;
    VBO* arrowVBO;
    EBO* arrowEBO;
    int arrowIndexCount;
    float cachedArrowCubeSize;
    
    // Limites e parâmetros
    float minDistance;
    float maxDistance;
    float scrollSensitivity;
    
    // Reconstrói mesh da seta quando tamanho muda
    void rebuildArrowMesh(float cubeSize);
};

#endif
