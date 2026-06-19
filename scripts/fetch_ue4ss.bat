@echo off
setlocal

set UE4SS_VERSION=3.0.1
set UE4SS_URL=https://github.com/UE4SS-RE/RE-UE4SS/releases/download/v%UE4SS_VERSION%/UE4SS_v%UE4SS_VERSION%.zip
set DEST=%~dp0..\vendor\UE4SS

echo [fetch_ue4ss] Téléchargement UE4SS v%UE4SS_VERSION%...
curl -L -o "%TEMP%\ue4ss.zip" "%UE4SS_URL%"
if errorlevel 1 (
    echo ERREUR : échec du téléchargement.
    exit /b 1
)

echo [fetch_ue4ss] Extraction vers %DEST%...
if not exist "%DEST%" mkdir "%DEST%"
powershell -NoProfile -Command "Expand-Archive -Path '%TEMP%\ue4ss.zip' -DestinationPath '%DEST%' -Force"
del "%TEMP%\ue4ss.zip"

echo [fetch_ue4ss] Terminé.
