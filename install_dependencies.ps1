# Script de instalação de dependências para Windows
# CG_3D_Project - UFMG
# Requer Windows 10/11 e PowerShell 5.1 ou superior

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Instalando dependências para CG_3D_Project" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Verifica se está rodando como administrador
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Host "Aviso: Algumas operações podem requerer privilégios de administrador." -ForegroundColor Yellow
    Write-Host "Se encontrar problemas, execute o PowerShell como Administrador." -ForegroundColor Yellow
    Write-Host ""
}

# Verifica se MSYS2 está instalado
$msys2Path = "C:\msys64"
$msys2Exists = Test-Path $msys2Path

if (-not $msys2Exists) {
    Write-Host "MSYS2 não encontrado em $msys2Path" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "MSYS2 é necessário para compilar este projeto no Windows." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Opções de instalação:" -ForegroundColor Cyan
    Write-Host "1. Instalação automática via winget (recomendado)" -ForegroundColor Green
    Write-Host "2. Instalação manual" -ForegroundColor Green
    Write-Host ""
    
    $choice = Read-Host "Deseja instalar MSYS2 automaticamente? (S/N)"
    
    if ($choice -eq "S" -or $choice -eq "s") {
        Write-Host ""
        Write-Host "Instalando MSYS2 via winget..." -ForegroundColor Cyan
        
        # Verifica se winget está disponível
        if (Get-Command winget -ErrorAction SilentlyContinue) {
            try {
                winget install --id MSYS2.MSYS2 --silent --accept-package-agreements --accept-source-agreements
                Write-Host "MSYS2 instalado com sucesso!" -ForegroundColor Green
                Write-Host ""
                Write-Host "IMPORTANTE: Após a instalação do MSYS2:" -ForegroundColor Yellow
                Write-Host "1. Feche e reabra o terminal" -ForegroundColor Yellow
                Write-Host "2. Execute este script novamente para instalar as dependências" -ForegroundColor Yellow
                Write-Host ""
                Write-Host "Ou instale manualmente as dependências no MSYS2:" -ForegroundColor Yellow
                Write-Host "  pacman -Syu" -ForegroundColor Cyan
                Write-Host "  pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw mingw-w64-x86_64-freetype mingw-w64-x86_64-glm pkg-config" -ForegroundColor Cyan
                exit 0
            }
            catch {
                Write-Host "Erro ao instalar MSYS2 via winget: $_" -ForegroundColor Red
                Write-Host ""
                Write-Host "Por favor, instale MSYS2 manualmente:" -ForegroundColor Yellow
                Write-Host "1. Baixe de: https://www.msys2.org/" -ForegroundColor Cyan
                Write-Host "2. Execute o instalador" -ForegroundColor Cyan
                Write-Host "3. Execute este script novamente" -ForegroundColor Cyan
                exit 1
            }
        }
        else {
            Write-Host "winget não encontrado. Por favor, instale MSYS2 manualmente:" -ForegroundColor Yellow
            Write-Host "1. Baixe de: https://www.msys2.org/" -ForegroundColor Cyan
            Write-Host "2. Execute o instalador" -ForegroundColor Cyan
            Write-Host "3. Execute este script novamente" -ForegroundColor Cyan
            exit 1
        }
    }
    else {
        Write-Host ""
        Write-Host "Por favor, instale MSYS2 manualmente:" -ForegroundColor Yellow
        Write-Host "1. Baixe de: https://www.msys2.org/" -ForegroundColor Cyan
        Write-Host "2. Execute o instalador" -ForegroundColor Cyan
        Write-Host "3. Execute este script novamente" -ForegroundColor Cyan
        exit 1
    }
}

# Verifica se o pacman está disponível (MSYS2 está instalado)
$pacmanPath = Join-Path $msys2Path "usr\bin\pacman.exe"

if (-not (Test-Path $pacmanPath)) {
    Write-Host "Erro: pacman não encontrado em $pacmanPath" -ForegroundColor Red
    Write-Host "Verifique se o MSYS2 foi instalado corretamente." -ForegroundColor Red
    exit 1
}

Write-Host "MSYS2 encontrado em $msys2Path" -ForegroundColor Green
Write-Host ""

# Atualiza o banco de dados de pacotes
Write-Host "Atualizando banco de dados de pacotes do MSYS2..." -ForegroundColor Cyan
& $pacmanPath -Sy

if ($LASTEXITCODE -ne 0) {
    Write-Host "Aviso: Erro ao atualizar pacotes. Continuando..." -ForegroundColor Yellow
}

# Instala as dependências
Write-Host ""
Write-Host "Instalando dependências de compilação e bibliotecas..." -ForegroundColor Cyan
Write-Host ""

$packages = @(
    "base-devel",
    "mingw-w64-x86_64-toolchain",
    "mingw-w64-x86_64-glfw",
    "mingw-w64-x86_64-freetype",
    "mingw-w64-x86_64-glm",
    "pkg-config"
)

$packagesString = $packages -join " "

& $pacmanPath -S --needed --noconfirm $packagesString

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Red
    Write-Host "Erro ao instalar dependências" -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Red
    Write-Host "Por favor, verifique as mensagens de erro acima." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Tente instalar manualmente no MSYS2 MinGW 64-bit:" -ForegroundColor Yellow
    Write-Host "  pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw mingw-w64-x86_64-freetype mingw-w64-x86_64-glm pkg-config" -ForegroundColor Cyan
    exit 1
}

# Verifica se as bibliotecas foram instaladas corretamente
Write-Host ""
Write-Host "Verificando instalação das dependências..." -ForegroundColor Cyan
Write-Host ""

$mingw64Path = Join-Path $msys2Path "mingw64"
$missingDeps = 0

# Verifica pkg-config
$pkgConfigPath = Join-Path $mingw64Path "bin\pkg-config.exe"
if (Test-Path $pkgConfigPath) {
    Write-Host "✓ pkg-config instalado" -ForegroundColor Green
}
else {
    Write-Host "✗ pkg-config não encontrado" -ForegroundColor Red
    $missingDeps = 1
}

# Verifica GLFW
$glfwPath = Join-Path $mingw64Path "lib\pkgconfig\glfw3.pc"
if (Test-Path $glfwPath) {
    Write-Host "✓ GLFW3 instalado" -ForegroundColor Green
}
else {
    Write-Host "✗ GLFW3 não encontrado" -ForegroundColor Red
    $missingDeps = 1
}

# Verifica FreeType
$freetypePath = Join-Path $mingw64Path "lib\pkgconfig\freetype2.pc"
if (Test-Path $freetypePath) {
    Write-Host "✓ FreeType2 instalado" -ForegroundColor Green
}
else {
    Write-Host "✗ FreeType2 não encontrado" -ForegroundColor Red
    $missingDeps = 1
}

# Verifica GLM
$glmPath = Join-Path $mingw64Path "include\glm"
if (Test-Path $glmPath) {
    Write-Host "✓ GLM encontrado" -ForegroundColor Green
}
else {
    Write-Host "✗ GLM não encontrado" -ForegroundColor Red
    $missingDeps = 1
}

# Verifica g++
$gppPath = Join-Path $mingw64Path "bin\g++.exe"
if (Test-Path $gppPath) {
    $gppVersion = & $gppPath --version 2>&1 | Select-Object -First 1
    Write-Host "✓ g++ instalado ($gppVersion)" -ForegroundColor Green
}
else {
    Write-Host "✗ g++ não encontrado" -ForegroundColor Red
    $missingDeps = 1
}

# Verifica make
$makePath = Join-Path $mingw64Path "bin\make.exe"
if (Test-Path $makePath) {
    Write-Host "✓ make instalado" -ForegroundColor Green
}
else {
    Write-Host "✗ make não encontrado" -ForegroundColor Red
    $missingDeps = 1
}

if ($missingDeps -eq 0) {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Green
    Write-Host "✓ Todas as dependências foram instaladas!" -ForegroundColor Green
    Write-Host "==========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Próximos passos:" -ForegroundColor Cyan
    Write-Host "1. Abra o MSYS2 MinGW 64-bit (não o MSYS2 normal)" -ForegroundColor Yellow
    Write-Host "2. Navegue até o diretório do projeto" -ForegroundColor Yellow
    Write-Host "3. Compile o projeto: make" -ForegroundColor Yellow
    Write-Host "4. Execute o projeto: ./demo.exe" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "IMPORTANTE: Use o terminal MSYS2 MinGW 64-bit para compilar!" -ForegroundColor Yellow
    Write-Host "Não use o PowerShell ou CMD diretamente." -ForegroundColor Yellow
    Write-Host ""
}
else {
    Write-Host ""
    Write-Host "==========================================" -ForegroundColor Red
    Write-Host "⚠ Algumas dependências podem estar faltando" -ForegroundColor Red
    Write-Host "==========================================" -ForegroundColor Red
    Write-Host "Por favor, verifique as mensagens acima e tente instalar" -ForegroundColor Yellow
    Write-Host "as dependências manualmente no MSYS2 MinGW 64-bit:" -ForegroundColor Yellow
    Write-Host "  pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw mingw-w64-x86_64-freetype mingw-w64-x86_64-glm pkg-config" -ForegroundColor Cyan
    exit 1
}

