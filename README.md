# CG-TP1-BOIDS

Projeto de Computação Gráfica feito utilizando OpenGL e C++ para simular o comportamento de BOIDS.

**Universidade Federal de Minas Gerais (UFMG)**

# Falta fazer:
[X] Tirar prints para a documentação do trabalho

[ ] Gravar um vídeo de apresentação para aumentar o leque de possibilidades de apresentação caso aconteça algum problema

[x] Revisar o trabalho, documentação

[ ] Enviar para avaliação

## 🎯 Plataformas Suportadas

- ✅ **Linux** (Ubuntu, Debian, Zorin OS e derivados)
- ❓ **Windows** (Windows 10/11 via MSYS2/MinGW ou talvez só com WSL)

## ⌨️ Controles

- **C**: Traz a luz para a posição da câmera
- **0**: Câmera livre (para de seguir o grupo de boids)
- **1**: Move câmera para o topo da torre
- **2**: Câmera fixa atrás do grupo de boids
- **3**: Câmera fixa ao lado do grupo de boids
- **4**: Câmera fixa do outro lado do grupo
- **L**: Trava/destrava orientação da câmera para o centro do grupo
- **P**: Pausa o jogo
- **[**: Debug - pula um frame a cada clique (imprime coordenadas)
- **U**: Cria novo boid perto do grupo (funciona até pausado)
- **-**: Remove um boid aleatório (mantém mínimo de 1)
- **I** e **O**: Controla velocidade do boid objetivo (verde grande)
- **N** e **M**: Controla velocidade do grupo de boids
- **F**: Ativa/desativa fog atmosférico
- **WASD**: Move a câmera
- **Ctrl**: Move câmera para baixo
- **Espaço**: Move câmera para cima
- **Shift**: Acelera movimento da câmera (combinar com acima)
- **Esc**: Sair

## 📦 Dependências

### Ubuntu/Debian/Zorin OS Linux

#### Instalação Automática (Recomendado)

```bash
chmod +x install_dependencies.sh
./install_dependencies.sh
```

#### Instalação Manual

```bash
sudo apt-get update
sudo apt-get install -y build-essential g++ make libglfw3-dev libfreetype6-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libglm-dev pkg-config zlib1g-dev
```

#### ⚠️ Observações Importantes

**Drivers Gráficos (CRÍTICO)**

Verifique se o OpenGL está funcionando:
```bash
glxinfo | grep "OpenGL version"
```

Se o comando não funcionar, instale:
```bash
sudo apt-get install -y mesa-utils
```

**Para NVIDIA:**
```bash
nvidia-smi  # Verifica se driver está instalado
sudo ubuntu-drivers autoinstall  # Instala automaticamente
```

**Para AMD/Intel:** Os drivers geralmente vêm pré-instalados. Se houver problemas:
```bash
sudo apt-get install -y mesa-utils
```

### Windows 10/11

#### Pré-requisito: MSYS2

O projeto requer **MSYS2** para compilar no Windows.

#### Instalação Automática (Recomendado)

1. **Instale o MSYS2:**
   ```powershell
   winget install --id MSYS2.MSYS2 --silent --accept-package-agreements --accept-source-agreements
   ```
   Ou baixe de https://www.msys2.org/

2. **Execute o script de instalação:**
   ```powershell
   .\install_dependencies.ps1
   ```

#### Instalação Manual

Abra o **terminal MSYS2 MinGW 64-bit**:

```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw mingw-w64-x86_64-freetype mingw-w64-x86_64-glm pkg-config
```

#### ⚠️ Observações Importantes para Windows

**Terminal Correto (CRÍTICO)**

**SEMPRE use o terminal MSYS2 MinGW 64-bit!**

- ❌ Não use: PowerShell, CMD, ou MSYS2 normal
- ✅ USE: MSYS2 MinGW 64-bit (procure no menu Iniciar)

**Caminhos no MSYS2:**
- Drive C: → `/c/`
- Drive D: → `/d/`

## 🔨 Compilação

### Linux/Windows (MSYS2):

```bash
make
```

### Limpeza:
```bash
make clean
```

## ▶️ Execução

### Linux/macOS:
```bash
./demo
```

### Windows (MSYS2):
```bash
./demo.exe
```

## 📋 Estrutura do Projeto

```
.
├── main.cpp                          # Entrada principal
├── headers/                          # Arquivos de cabeçalho
│   ├── game/                        # Sistema de jogo
│   │   ├── gameController.hpp      # Controlador principal
│   │   ├── globals.hpp             # Variáveis globais
│   │   ├── utils.hpp               # Utilitários
│   │   ├── vectors.hpp             # Classes de vetores
│   │   └── objects/                # Objetos do jogo
│   │       ├── GObject.hpp         # Objeto gráfico base
│   │       ├── boid.hpp            # Classe Boid
│   │       ├── tower.hpp           # Torre
│   │       └── customObjects.hpp   # Objetos customizados
│   └── graphics/                    # Sistema gráfico
│       ├── camera.hpp              # Câmera 3D
│       ├── shaderClass.hpp         # Gerenciador de shaders
│       ├── texture.hpp             # Gerenciador de texturas
│       ├── VAO.hpp                 # Vertex Array Object
│       ├── VBO.hpp                 # Vertex Buffer Object
│       ├── EBO.hpp                 # Element Buffer Object
│       └── GText.hpp               # Renderizador de texto
├── src/                             # Implementações
│   ├── game/                       # Implementações do jogo
│   ├── graphics/                   # Implementações gráficas
│   └── objects/                    # Implementações de objetos
├── shaders/                         # Arquivos GLSL
│   ├── default.vert/frag           # Shader padrão
│   ├── light.vert/frag             # Shader de iluminação
│   └── text.vert/frag              # Shader de texto
├── Libraries/                       # Bibliotecas externas
│   └── include/                    # Headers de bibliotecas
├── textures/                        # Arquivos de textura
└── Makefile                         # Arquivo de compilação
```

## 🎮 Simulação BOIDS

Este projeto implementa um sistema completo de simulação de comportamento coletivo inspirado no movimento de pássaros (BOIDS - Bird-like Objects). O sistema inclui:

- **Separação**: Evita aglomeração com outros boids
- **Alinhamento**: Alinha velocidade com boids vizinhos
- **Coesão**: Move em direção ao centro do grupo
- **Objetivo**: Segue um boid objetivo (verde grande - goal boid)
- **Guia Invisível**: Boid fantasma (ghost boid) que orienta o comportamento coletivo
- **Obstáculos**: Desvia de torres cilíndricas e paredes usando detecção de colisão
- **Sombras**: Renderiza sombras de boids com efeito de asas e sombras dos obstáculos
- **Fog Atmosférico**: Efeito de profundidade via fog exponencial ativável em tempo real
- **Iluminação Phong**: Modelo de iluminação real-time com preview interativo

## 📝 Notas de Desenvolvimento

### Recursos Principais Implementados

**Funcionalidades Básicas (6/6 - 100%)**
- ✅ Mundo 3D com obstáculos (torres)
- ✅ Quatro modos de câmera dinâmica
- ✅ Iluminação Phong com Preview interativo (tecla C)
- ✅ Boids renderizados como pirâmides com animação de asas
- ✅ Criação e remoção dinâmica de boids (U e MINUS)
- ✅ Animação independente de asas para cada boid

**Funcionalidades Extras (6/6 - 100%)**
- ✅ Obstáculos com colisão inteligente e desvio
- ✅ Sombras de boids (com efeito de asas) e sombras de obstáculos
- ✅ Fog atmosférico exponencial (tecla F)
- ✅ Modo pausa com debug step-by-step (tecla P e [)
- ✅ Redimensionamento responsivo de janela
- ✅ Banking simplificado (inclinação em curvas)

### Tecnologia

- O código usa **GLM** para operações de álgebra linear
- **FreeType** é usado para renderização de texto
- **GLAD** carrega funções OpenGL dynamicamente
- **GLFW** gerencia janela, eventos e contexto OpenGL
- **C++11** com suporte a RAII e memory safety

## 🐛 Resolução de Problemas

Se o projeto não compilar, verifique:

1. **Linux**: Drivers gráficos instalados (`glxinfo` retorna versão)
2. **Windows**: Usando terminal MSYS2 MinGW 64-bit correto
3. Todas as dependências instaladas via scripts
4. Versão do C++ é compatível (usa C++11)

## 📄 Licença

Este é um projeto acadêmico para a disciplina de Computação Gráfica da UFMG.

## 👥 Autores

Desenvolvido para fins educacionais - UFMG
