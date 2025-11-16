#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "shaderClass.hpp"

typedef glm::vec3 v3;
typedef glm::vec4 v4;
typedef glm::mat4 m4;

// Classe que representa a câmera 3D da cena
class Camera {
    public:
    // Posição da câmera no espaço mundial
    v3 position;
    
    // Direção para onde a câmera está olhando
    v3 orientation = v3(0.0f, 0.0f, 1.0f);
    
    // Vetor "para cima" para manter a orientação correta
    v3 up = v3(0.0f, 1.0f, 0.0f);
    
    // Matriz da câmera para transformação
    m4 cameraMatrix = m4(1.0f);

    // Dimensões da janela
    int width;
    int height;

    // Velocidade de movimento da câmera
    float speed = 0.4f;
    
    // Sensibilidade do mouse para rotação
    float sensitivity = 100.0f;

    // Rastreia se é o primeiro clique do mouse
    bool firstClick = true;

    // Construtor que inicializa a câmera com posição e dimensões
    Camera(int width, int height, v3 position);

    // Atualiza a matriz de projeção
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    
    // Envia a matriz da câmera para o shader
    void matrix(Shader *shader, const char* uniform);

    // Atualiza a matriz e envia para o shader em uma única chamada
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, const char* uniform);
    
    // Processa entrada do usuário para movimento da câmera
    void inputs(GLFWwindow* window);

    // Faz a câmera olhar para um alvo específico
    void lookAt(v3 target);
};

#endif