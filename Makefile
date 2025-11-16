# Arquivos que serão compilados
OBJS = main.cpp Libraries/glad.cpp src/game/*.cpp src/graphics/*.cpp src/game/objects/*.cpp

# Compilador utilizado
CC = g++

# Nome do executável
EXE_NAME = demo

# Detecta o sistema operacional
# No Windows com MSYS2/MinGW, uname retorna MINGW64_NT ou MSYS_NT
UNAME_S := $(shell uname -s 2>/dev/null)

# Fallback para Windows se uname não estiver disponível
ifeq ($(UNAME_S),)
    ifeq ($(OS),Windows_NT)
        UNAME_S := Windows_NT
    endif
endif

# Configuração para Linux
ifeq ($(UNAME_S),Linux)
    # Usa pkg-config para encontrar bibliotecas
    GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
    GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null)
    FREETYPE_CFLAGS := $(shell pkg-config --cflags freetype2 2>/dev/null)
    FREETYPE_LIBS := $(shell pkg-config --libs freetype2 2>/dev/null)
    
    # Procura por GLM em locais comuns (tenta múltiplos caminhos)
    GLM_INCLUDE := $(shell \
        if [ -d /usr/include/glm ]; then \
            echo "-I/usr/include"; \
        elif [ -d /usr/local/include/glm ]; then \
            echo "-I/usr/local/include"; \
        elif [ -d /usr/include/glm/glm ]; then \
            echo "-I/usr/include"; \
        else \
            echo ""; \
        fi)
    
    # Caminhos de incluir para Linux
    INCLUDE_PATHS = $(GLM_INCLUDE) -Iheaders/ -Iheaders/graphics/ -Iheaders/game/ -Iheaders/game/objects -ILibraries/include/
    
    # Caminhos de bibliotecas para Linux (usando bibliotecas do sistema)
    LIBRARY_PATHS = $(GLFW_LIBS) $(FREETYPE_LIBS) -lz
    
    # Flags do linker para Linux
    LINKER_FLAGS = -lGL -lX11 -lpthread -lXrandr -lXi -ldl
    
    # Flags do compilador
    COMPILER_FLAGS = $(GLFW_CFLAGS) $(FREETYPE_CFLAGS)
    
    # Aviso se as bibliotecas não forem encontradas
    ifeq ($(GLFW_LIBS),)
        $(warning Aviso: GLFW não encontrado via pkg-config. Certifique-se de que libglfw3-dev está instalado.)
    endif
    ifeq ($(FREETYPE_LIBS),)
        $(warning Aviso: FreeType não encontrado via pkg-config. Certifique-se de que libfreetype6-dev está instalado.)
    endif
endif

# Configuração para Windows (MSYS2/MinGW) - Verifica primeiro as variantes MSYS2
ifeq ($(UNAME_S),MINGW64_NT)
    UNAME_S := Windows_NT
endif
ifeq ($(UNAME_S),MSYS_NT)
    UNAME_S := Windows_NT
endif
ifeq ($(UNAME_S),Windows_NT)
    # Usa pkg-config para bibliotecas no Windows (MSYS2)
    GLFW_CFLAGS := $(shell pkg-config --cflags glfw3 2>/dev/null)
    GLFW_LIBS := $(shell pkg-config --libs glfw3 2>/dev/null)
    FREETYPE_CFLAGS := $(shell pkg-config --cflags freetype2 2>/dev/null)
    FREETYPE_LIBS := $(shell pkg-config --libs freetype2 2>/dev/null)
    
    # Procura por GLM em locais MSYS2/MinGW
    GLM_INCLUDE := $(shell \
        if [ -d /mingw64/include/glm ]; then \
            echo "-I/mingw64/include"; \
        elif [ -d /usr/include/glm ]; then \
            echo "-I/usr/include"; \
        elif [ -d /mingw32/include/glm ]; then \
            echo "-I/mingw32/include"; \
        else \
            echo ""; \
        fi)
    
    # Caminhos de incluir para Windows
    INCLUDE_PATHS = $(GLM_INCLUDE) -Iheaders/ -Iheaders/graphics/ -Iheaders/game/ -Iheaders/game/objects -ILibraries/include/
    
    # Caminhos de bibliotecas para Windows (usando bibliotecas MSYS2/MinGW)
    LIBRARY_PATHS = $(GLFW_LIBS) $(FREETYPE_LIBS) -lz -lopengl32 -lgdi32
    
    # Flags do linker para Windows
    LINKER_FLAGS = -static-libgcc -static-libstdc++
    
    # Flags do compilador
    COMPILER_FLAGS = $(GLFW_CFLAGS) $(FREETYPE_CFLAGS)
    
    # Nome do executável para Windows
    EXE_NAME = demo.exe
    
    # Aviso se as bibliotecas não forem encontradas
    ifeq ($(GLFW_LIBS),)
        $(warning Aviso: GLFW não encontrado via pkg-config. Certifique-se de que mingw-w64-x86_64-glfw está instalado no MSYS2.)
    endif
    ifeq ($(FREETYPE_LIBS),)
        $(warning Aviso: FreeType não encontrado via pkg-config. Certifique-se de que mingw-w64-x86_64-freetype está instalado no MSYS2.)
    endif
endif

# Erro se o SO não for detectado
ifneq ($(UNAME_S),Linux)
ifneq ($(UNAME_S),Windows_NT)
    $(error Sistema operacional não suportado: $(UNAME_S). Este projeto suporta Linux e Windows (via MSYS2/MinGW).)
endif
endif

# Alvo que compila o executável
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -std=c++11 -o $(EXE_NAME)

# Alvo para limpeza
clean:
ifeq ($(UNAME_S),Windows_NT)
	rm -f $(EXE_NAME) $(EXE_NAME:.exe=)
else
	rm -f $(EXE_NAME)
endif