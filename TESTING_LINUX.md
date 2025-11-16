# Guia de Testes e Validação - Linux

Este documento contém instruções para validar se todas as dependências estão instaladas corretamente antes de tentar compilar e executar o projeto no Linux.

## Pré-requisitos

Antes de começar, certifique-se de que:
- Você está em um sistema Linux (Ubuntu, Debian, Zorin OS ou derivados)
- Você tem acesso ao terminal
- Você tem privilégios sudo (para instalação de pacotes)

## 1. Verificação do Sistema Operacional

```bash
uname -a
```

**Resultado esperado:** Deve mostrar informações do Linux (kernel, arquitetura, etc.)

## 2. Verificação do Compilador (g++)

```bash
g++ --version
```

**Resultado esperado:** Deve mostrar a versão do GCC (ex: `g++ (Ubuntu 11.x.x) 11.x.x`)

**Se não estiver instalado:**
```bash
sudo apt-get install -y build-essential g++
```

## 3. Verificação do Make

```bash
make --version
```

**Resultado esperado:** Deve mostrar a versão do Make (ex: `GNU Make 4.x`)

**Se não estiver instalado:**
```bash
sudo apt-get install -y make
```

## 4. Verificação do pkg-config

```bash
pkg-config --version
```

**Resultado esperado:** Deve mostrar a versão do pkg-config (ex: `0.29.x`)

**Se não estiver instalado:**
```bash
sudo apt-get install -y pkg-config
```

## 5. Verificação do GLFW3

```bash
pkg-config --modversion glfw3
```

**Resultado esperado:** Deve mostrar a versão do GLFW (ex: `3.3.x`)

**Se não estiver instalado:**
```bash
sudo apt-get install -y libglfw3-dev
```

**Teste adicional:**
```bash
pkg-config --cflags glfw3
pkg-config --libs glfw3
```

Ambos os comandos devem retornar flags de compilação e linking sem erros.

## 6. Verificação do FreeType2

```bash
pkg-config --modversion freetype2
```

**Resultado esperado:** Deve mostrar a versão do FreeType (ex: `2.11.x`)

**Se não estiver instalado:**
```bash
sudo apt-get install -y libfreetype6-dev
```

**Teste adicional:**
```bash
pkg-config --cflags freetype2
pkg-config --libs freetype2
```

Ambos os comandos devem retornar flags de compilação e linking sem erros.

## 7. Verificação do GLM

```bash
ls -d /usr/include/glm 2>/dev/null || ls -d /usr/local/include/glm 2>/dev/null
```

**Resultado esperado:** Deve mostrar o caminho do diretório GLM (ex: `/usr/include/glm`)

**Se não estiver instalado:**
```bash
sudo apt-get install -y libglm-dev
```

**Teste adicional:**
```bash
ls /usr/include/glm/glm.hpp 2>/dev/null || ls /usr/local/include/glm/glm.hpp 2>/dev/null
```

Deve encontrar o arquivo `glm.hpp`.

## 8. Verificação do OpenGL (Mesa)

```bash
glxinfo | grep "OpenGL version"
```

**Resultado esperado:** Deve mostrar a versão do OpenGL (ex: `OpenGL version string: 4.6.x`)

**Se o comando não funcionar:**
```bash
sudo apt-get install -y mesa-utils
glxinfo | grep "OpenGL version"
```

**Alternativa (se glxinfo não estiver disponível):**
```bash
ls /usr/include/GL/gl.h 2>/dev/null || ls /usr/include/GLES/gl.h 2>/dev/null
```

Deve encontrar pelo menos um dos arquivos de header do OpenGL.

## 9. Verificação das Bibliotecas X11

```bash
ls /usr/include/X11/Xlib.h 2>/dev/null
```

**Resultado esperado:** Deve encontrar o arquivo

**Se não estiver instalado:**
```bash
sudo apt-get install -y libx11-dev libxrandr-dev libxi-dev
```

## 10. Verificação do zlib

```bash
ls /usr/include/zlib.h 2>/dev/null
```

**Resultado esperado:** Deve encontrar o arquivo

**Se não estiver instalado:**
```bash
sudo apt-get install -y zlib1g-dev
```

## 11. Teste de Compilação Rápida

Crie um arquivo de teste para verificar se todas as bibliotecas podem ser linkadas:

```bash
cat > test_compile.cpp << 'EOF'
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW" << std::endl;
        return 1;
    }
    std::cout << "GLFW inicializado com sucesso!" << std::endl;
    std::cout << "GLM versão: " << GLM_VERSION_MAJOR << "." << GLM_VERSION_MINOR << std::endl;
    glfwTerminate();
    return 0;
}
EOF

g++ test_compile.cpp -o test_compile $(pkg-config --cflags --libs glfw3) $(pkg-config --cflags --libs freetype2) -I/usr/include/glm -lGL -lX11 -lpthread -lXrandr -lXi -ldl -std=c++17

if [ $? -eq 0 ]; then
    echo "✓ Compilação de teste bem-sucedida!"
    ./test_compile
    rm -f test_compile test_compile.cpp
else
    echo "✗ Erro na compilação de teste. Verifique as dependências acima."
    exit 1
fi
```

**Resultado esperado:** Deve compilar sem erros e executar mostrando "GLFW inicializado com sucesso!"

## 12. Verificação do Script de Instalação

Se você usou o script de instalação automática:

```bash
./install_dependencies.sh
```

O script deve:
- Instalar todas as dependências
- Verificar cada biblioteca
- Mostrar mensagens de sucesso (✓) para cada componente

## Checklist Final

Antes de compilar o projeto, verifique se todos estes itens estão marcados:

- [ ] Sistema operacional Linux detectado
- [ ] g++ instalado e funcionando
- [ ] make instalado e funcionando
- [ ] pkg-config instalado e funcionando
- [ ] GLFW3 instalado e detectado pelo pkg-config
- [ ] FreeType2 instalado e detectado pelo pkg-config
- [ ] GLM instalado e encontrado em /usr/include/glm ou /usr/local/include/glm
- [ ] OpenGL (Mesa) funcionando (glxinfo mostra versão)
- [ ] Bibliotecas X11 instaladas
- [ ] zlib instalado
- [ ] Teste de compilação bem-sucedido

## Próximos Passos

Se todos os testes passaram:

1. Navegue até o diretório do projeto
2. Execute: `make`
3. Se compilar com sucesso, execute: `./demo`

## Resolução de Problemas

### Erro: "GLFW não encontrado via pkg-config"
```bash
sudo apt-get install -y libglfw3-dev
pkg-config --modversion glfw3  # Verificar novamente
```

### Erro: "FreeType não encontrado via pkg-config"
```bash
sudo apt-get install -y libfreetype6-dev
pkg-config --modversion freetype2  # Verificar novamente
```

### Erro: "GLM não encontrado"
```bash
sudo apt-get install -y libglm-dev
ls /usr/include/glm  # Verificar se existe
```

### Erro: "OpenGL version not found"
```bash
sudo apt-get install -y mesa-utils
glxinfo | grep "OpenGL version"
# Se ainda não funcionar, verifique os drivers gráficos
```

### Erro de compilação relacionado a bibliotecas faltando
Execute o script de instalação novamente:
```bash
./install_dependencies.sh
```

## Notas Importantes

- **Drivers Gráficos:** Em instalações limpas, os drivers gráficos podem não estar instalados. Veja o README.md para instruções específicas sobre drivers NVIDIA, AMD ou Intel.

- **Ambiente Gráfico:** Este projeto requer um ambiente gráfico (X11 ou Wayland) rodando. Não funciona em modo texto puro.

- **Permissões:** Alguns comandos de verificação podem requerer privilégios sudo para instalação, mas não para verificação.

