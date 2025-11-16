#!/bin/bash

# Script de instalação de dependências para Ubuntu/Debian Linux
# CG_3D_Project - UFMG
# Compatível com Zorin OS, Ubuntu, Debian e derivados

echo "=========================================="
echo "Instalando dependências para CG_3D_Project"
echo "=========================================="
echo ""

# Verifica se está rodando em Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Aviso: Este script é destinado para sistemas Linux (Ubuntu/Debian/Zorin OS)"
    echo "Sistema detectado: $OSTYPE"
    read -p "Deseja continuar mesmo assim? (s/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Ss]$ ]]; then
        exit 1
    fi
fi

# Verifica se o usuário tem permissões sudo
if ! sudo -n true 2>/dev/null; then
    echo "Este script requer privilégios de administrador (sudo)."
    echo "Você será solicitado a inserir sua senha."
    echo ""
fi

# Verifica se apt-get está disponível
if ! command -v apt-get &> /dev/null; then
    echo "Erro: apt-get não encontrado. Este script é para sistemas baseados em Debian/Ubuntu."
    echo "Por favor, instale as dependências manualmente conforme o README.md"
    exit 1
fi

# Atualiza a lista de pacotes
echo "Atualizando lista de pacotes..."
sudo apt-get update

# Instala as dependências
echo ""
echo "Instalando dependências de compilação e bibliotecas..."
sudo apt-get install -y \
    build-essential \
    g++ \
    make \
    libglfw3-dev \
    libfreetype6-dev \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libglm-dev \
    pkg-config \
    zlib1g-dev

# Verifica se a instalação foi bem-sucedida
if [ $? -ne 0 ]; then
    echo ""
    echo "=========================================="
    echo "✗ Erro ao instalar dependências"
    echo "=========================================="
    echo "Por favor, verifique as mensagens de erro acima."
    exit 1
fi

# Verifica se as bibliotecas principais foram instaladas corretamente
echo ""
echo "Verificando instalação das dependências..."
echo ""

MISSING_DEPS=0

# Verifica pkg-config
if ! command -v pkg-config &> /dev/null; then
    echo "✗ pkg-config não encontrado"
    MISSING_DEPS=1
else
    echo "✓ pkg-config instalado"
fi

# Verifica GLFW
if ! pkg-config --exists glfw3 2>/dev/null; then
    echo "✗ libglfw3-dev não encontrado ou não configurado corretamente"
    MISSING_DEPS=1
else
    echo "✓ GLFW3 instalado ($(pkg-config --modversion glfw3))"
fi

# Verifica FreeType
if ! pkg-config --exists freetype2 2>/dev/null; then
    echo "✗ libfreetype6-dev não encontrado ou não configurado corretamente"
    MISSING_DEPS=1
else
    echo "✓ FreeType2 instalado ($(pkg-config --modversion freetype2))"
fi

# Verifica GLM
if [ -d "/usr/include/glm" ] || [ -d "/usr/local/include/glm" ]; then
    echo "✓ GLM encontrado"
else
    echo "✗ GLM não encontrado em /usr/include/glm ou /usr/local/include/glm"
    MISSING_DEPS=1
fi

# Verifica g++
if ! command -v g++ &> /dev/null; then
    echo "✗ g++ não encontrado"
    MISSING_DEPS=1
else
    echo "✓ g++ instalado ($(g++ --version | head -n1))"
fi

# Verifica make
if ! command -v make &> /dev/null; then
    echo "✗ make não encontrado"
    MISSING_DEPS=1
else
    echo "✓ make instalado"
fi

# Verifica OpenGL (Mesa)
if [ -f "/usr/include/GL/gl.h" ] || [ -f "/usr/include/GLES/gl.h" ]; then
    echo "✓ OpenGL headers encontrados"
else
    echo "⚠ Aviso: Headers do OpenGL não encontrados (pode estar em outro local)"
fi

if [ $MISSING_DEPS -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✓ Todas as dependências foram instaladas!"
    echo "=========================================="
    echo ""
    echo "Próximos passos:"
    echo "  1. Compile o projeto: make"
    echo "  2. Execute o projeto: ./demo"
    echo ""
    echo "Nota: Se você estiver usando uma instalação limpa do Zorin OS,"
    echo "certifique-se de que os drivers gráficos estão instalados."
    echo "Veja o README.md para mais informações."
    echo ""
else
    echo ""
    echo "=========================================="
    echo "⚠ Algumas dependências podem estar faltando"
    echo "=========================================="
    echo "Por favor, verifique as mensagens acima e tente instalar"
    echo "as dependências manualmente se necessário."
    exit 1
fi

