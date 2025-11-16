# Guia de Testes e Validação - Windows

Este documento contém instruções para validar se todas as dependências estão instaladas corretamente antes de tentar compilar e executar o projeto no Windows 11.

## Pré-requisitos

Antes de começar, certifique-se de que:
- Você está em um sistema Windows 10 ou 11
- Você tem acesso ao PowerShell
- Você tem privilégios de administrador (para instalação de software)

## 1. Verificação do Sistema Operacional

```powershell
$PSVersionTable.OS
```

**Resultado esperado:** Deve mostrar informações do Windows (ex: `Microsoft Windows 10.0.xxxxx` ou `Microsoft Windows 11.0.xxxxx`)

## 2. Verificação do MSYS2

O MSYS2 é **obrigatório** para compilar este projeto no Windows. Ele fornece um ambiente Unix-like com todas as ferramentas necessárias.

```powershell
Test-Path "C:\msys64"
```

**Resultado esperado:** Deve retornar `True`

**Se não estiver instalado:**

### Opção 1: Instalação Automática (Recomendado)

```powershell
# Verifica se winget está disponível
if (Get-Command winget -ErrorAction SilentlyContinue) {
    winget install --id MSYS2.MSYS2 --silent --accept-package-agreements --accept-source-agreements
}
```

### Opção 2: Instalação Manual

1. Baixe o instalador do MSYS2: https://www.msys2.org/
2. Execute o instalador e instale em `C:\msys64` (padrão)
3. Após a instalação, **feche e reabra o terminal**

**Após instalar o MSYS2, execute o script de instalação:**
```powershell
.\install_dependencies.ps1
```

## 3. Verificação do MSYS2 MinGW 64-bit

**IMPORTANTE:** Você deve usar o terminal **MSYS2 MinGW 64-bit**, não o MSYS2 normal ou o PowerShell/CMD diretamente.

1. Abra o menu Iniciar
2. Procure por "MSYS2 MinGW 64-bit"
3. Abra o terminal

No terminal MSYS2 MinGW 64-bit, verifique:

```bash
uname -a
```

**Resultado esperado:** Deve mostrar `MINGW64_NT` ou similar, indicando que está no ambiente MinGW correto.

## 4. Verificação do Compilador (g++)

No terminal **MSYS2 MinGW 64-bit**:

```bash
g++ --version
```

**Resultado esperado:** Deve mostrar a versão do GCC (ex: `g++ (Rev10, Built by MSYS2 project) 13.x.x`)

**Se não estiver instalado:**
```bash
pacman -S --needed mingw-w64-x86_64-toolchain
```

## 5. Verificação do Make

No terminal **MSYS2 MinGW 64-bit**:

```bash
make --version
```

**Resultado esperado:** Deve mostrar a versão do Make (ex: `GNU Make 4.x`)

**Se não estiver instalado:**
```bash
pacman -S --needed base-devel
```

## 6. Verificação do pkg-config

No terminal **MSYS2 MinGW 64-bit**:

```bash
pkg-config --version
```

**Resultado esperado:** Deve mostrar a versão do pkg-config (ex: `1.x.x`)

**Se não estiver instalado:**
```bash
pacman -S --needed pkg-config
```

## 7. Verificação do GLFW3

No terminal **MSYS2 MinGW 64-bit**:

```bash
pkg-config --modversion glfw3
```

**Resultado esperado:** Deve mostrar a versão do GLFW (ex: `3.3.x`)

**Se não estiver instalado:**
```bash
pacman -S --needed mingw-w64-x86_64-glfw
```

**Teste adicional:**
```bash
pkg-config --cflags glfw3
pkg-config --libs glfw3
```

Ambos os comandos devem retornar flags de compilação e linking sem erros.

## 8. Verificação do FreeType2

No terminal **MSYS2 MinGW 64-bit**:

```bash
pkg-config --modversion freetype2
```

**Resultado esperado:** Deve mostrar a versão do FreeType (ex: `2.13.x`)

**Se não estiver instalado:**
```bash
pacman -S --needed mingw-w64-x86_64-freetype
```

**Teste adicional:**
```bash
pkg-config --cflags freetype2
pkg-config --libs freetype2
```

Ambos os comandos devem retornar flags de compilação e linking sem erros.

## 9. Verificação do GLM

No terminal **MSYS2 MinGW 64-bit**:

```bash
ls /mingw64/include/glm 2>/dev/null || ls /usr/include/glm 2>/dev/null
```

**Resultado esperado:** Deve mostrar o caminho do diretório GLM (ex: `/mingw64/include/glm`)

**Se não estiver instalado:**
```bash
pacman -S --needed mingw-w64-x86_64-glm
```

**Teste adicional:**
```bash
ls /mingw64/include/glm/glm.hpp 2>/dev/null || ls /usr/include/glm/glm.hpp 2>/dev/null
```

Deve encontrar o arquivo `glm.hpp`.

## 10. Verificação do OpenGL

No Windows, o OpenGL geralmente vem pré-instalado com os drivers gráficos. Verifique:

No terminal **MSYS2 MinGW 64-bit**:

```bash
ls /mingw64/lib/libopengl32.a 2>/dev/null || ls /mingw64/lib/libopengl32.dll.a 2>/dev/null
```

**Resultado esperado:** Deve encontrar pelo menos um dos arquivos

**Nota:** O OpenGL no Windows é fornecido pelo sistema operacional e pelos drivers gráficos. Não é necessário instalar bibliotecas adicionais via pacman.

## 11. Verificação do zlib

No terminal **MSYS2 MinGW 64-bit**:

```bash
ls /mingw64/include/zlib.h 2>/dev/null || ls /usr/include/zlib.h 2>/dev/null
```

**Resultado esperado:** Deve encontrar o arquivo

**Se não estiver instalado:**
```bash
pacman -S --needed mingw-w64-x86_64-zlib
```

## 12. Teste de Compilação Rápida

No terminal **MSYS2 MinGW 64-bit**, crie um arquivo de teste:

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

g++ test_compile.cpp -o test_compile.exe $(pkg-config --cflags --libs glfw3) $(pkg-config --cflags --libs freetype2) -I/mingw64/include/glm -lopengl32 -lgdi32 -std=c++17

if [ $? -eq 0 ]; then
    echo "✓ Compilação de teste bem-sucedida!"
    ./test_compile.exe
    rm -f test_compile.exe test_compile.cpp
else
    echo "✗ Erro na compilação de teste. Verifique as dependências acima."
    exit 1
fi
```

**Resultado esperado:** Deve compilar sem erros e executar mostrando "GLFW inicializado com sucesso!"

## 13. Verificação do Script de Instalação

Se você usou o script PowerShell de instalação:

```powershell
.\install_dependencies.ps1
```

O script deve:
- Verificar se o MSYS2 está instalado
- Instalar todas as dependências via pacman
- Verificar cada biblioteca
- Mostrar mensagens de sucesso (✓) para cada componente

**Nota:** O script PowerShell instala as dependências, mas você ainda precisa usar o terminal MSYS2 MinGW 64-bit para compilar.

## Checklist Final

Antes de compilar o projeto, verifique se todos estes itens estão marcados:

- [ ] Sistema operacional Windows 10/11 detectado
- [ ] MSYS2 instalado em C:\msys64
- [ ] Terminal MSYS2 MinGW 64-bit aberto e funcionando
- [ ] g++ instalado e funcionando (verificado no MSYS2 MinGW 64-bit)
- [ ] make instalado e funcionando (verificado no MSYS2 MinGW 64-bit)
- [ ] pkg-config instalado e funcionando (verificado no MSYS2 MinGW 64-bit)
- [ ] GLFW3 instalado e detectado pelo pkg-config
- [ ] FreeType2 instalado e detectado pelo pkg-config
- [ ] GLM instalado e encontrado em /mingw64/include/glm
- [ ] OpenGL disponível (libopengl32 encontrado)
- [ ] zlib instalado
- [ ] Teste de compilação bem-sucedido no MSYS2 MinGW 64-bit

## Próximos Passos

Se todos os testes passaram:

1. Abra o terminal **MSYS2 MinGW 64-bit** (não o PowerShell!)
2. Navegue até o diretório do projeto:
   ```bash
   cd /c/caminho/para/o/projeto
   ```
   (Use `/c/` para o drive C:, `/d/` para o drive D:, etc.)
3. Execute: `make`
4. Se compilar com sucesso, execute: `./demo.exe`

## Resolução de Problemas

### Erro: "MSYS2 não encontrado"
- Instale o MSYS2 usando o script PowerShell ou manualmente
- Certifique-se de que está instalado em `C:\msys64`

### Erro: "GLFW não encontrado via pkg-config"
```bash
pacman -S --needed mingw-w64-x86_64-glfw
pkg-config --modversion glfw3  # Verificar novamente
```

### Erro: "FreeType não encontrado via pkg-config"
```bash
pacman -S --needed mingw-w64-x86_64-freetype
pkg-config --modversion freetype2  # Verificar novamente
```

### Erro: "GLM não encontrado"
```bash
pacman -S --needed mingw-w64-x86_64-glm
ls /mingw64/include/glm  # Verificar se existe
```

### Erro: "make: command not found"
```bash
pacman -S --needed base-devel
```

### Erro: "g++: command not found"
```bash
pacman -S --needed mingw-w64-x86_64-toolchain
```

### Erro de compilação: "cannot find -lopengl32"
- O OpenGL no Windows é fornecido pelo sistema
- Certifique-se de que os drivers gráficos estão instalados
- Verifique se está usando o terminal MSYS2 MinGW 64-bit (não o MSYS2 normal)

### Erro: "Failed to create window" ao executar
- Verifique se os drivers gráficos estão instalados e atualizados
- Certifique-se de que está executando em um ambiente gráfico (não SSH/remoto)

## Notas Importantes

- **Terminal Correto:** Sempre use o **MSYS2 MinGW 64-bit** para compilar. Não use o PowerShell, CMD, ou o terminal MSYS2 normal.

- **Caminhos:** No MSYS2, use caminhos Unix-style. O drive C: vira `/c/`, D: vira `/d/`, etc.

- **Drivers Gráficos:** Certifique-se de que os drivers gráficos estão instalados e atualizados. O Windows geralmente instala drivers básicos automaticamente, mas drivers específicos do fabricante podem ser necessários para melhor desempenho.

- **Executável:** O executável gerado será `demo.exe` (não apenas `demo`).

- **DLLs:** Se você mover o executável para outro computador, pode ser necessário copiar as DLLs do MinGW também, ou usar linking estático (já configurado no Makefile).

## Alternativa: Visual Studio

Se preferir usar Visual Studio em vez de MSYS2/MinGW, você precisará:
1. Instalar Visual Studio 2019 ou superior com suporte a C++
2. Instalar vcpkg
3. Instalar as dependências via vcpkg
4. Criar um projeto Visual Studio ou usar CMake

Esta abordagem não está coberta neste guia, mas é uma alternativa viável.

