@echo off
setlocal
set ROOT=%~dp0..
set BUILD_DIR=%ROOT%\build

echo [build] Configuration CMake...
cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo ERREUR : configuration CMake échouée.
    exit /b 1
)

echo [build] Compilation...
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 (
    echo ERREUR : compilation échouée.
    exit /b 1
)

echo [build] DLLs disponibles dans %ROOT%\out\
