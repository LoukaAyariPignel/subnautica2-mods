@echo off
setlocal
:: Usage: install_mod.bat <NomDuMod> <CheminJeu>
:: Exemple: install_mod.bat InfiniteOxygen "C:\Program Files\Steam\steamapps\common\Subnautica2"

set MOD_NAME=%1
set GAME_DIR=%2

if "%MOD_NAME%"=="" (
    echo Usage: install_mod.bat ^<NomDuMod^> ^<CheminJeu^>
    exit /b 1
)
if "%GAME_DIR%"=="" (
    echo Usage: install_mod.bat ^<NomDuMod^> ^<CheminJeu^>
    exit /b 1
)

set MOD_DEST=%GAME_DIR%\Mods\%MOD_NAME%
set ROOT=%~dp0..

:: Mod C++
if exist "%ROOT%\out\%MOD_NAME%\%MOD_NAME%.dll" (
    echo [install] Copie DLL C++ -> %MOD_DEST%
    if not exist "%MOD_DEST%" mkdir "%MOD_DEST%"
    copy /Y "%ROOT%\out\%MOD_NAME%\%MOD_NAME%.dll" "%MOD_DEST%\"
    copy /Y "%ROOT%\mods\cpp\%MOD_NAME%\mod.manifest" "%MOD_DEST%\"
    goto :done
)

:: Mod Lua
if exist "%ROOT%\mods\lua\%MOD_NAME%\Scripts\main.lua" (
    echo [install] Copie mod Lua -> %MOD_DEST%
    if not exist "%MOD_DEST%\Scripts" mkdir "%MOD_DEST%\Scripts"
    xcopy /E /Y "%ROOT%\mods\lua\%MOD_NAME%\Scripts\" "%MOD_DEST%\Scripts\"
    copy /Y "%ROOT%\mods\lua\%MOD_NAME%\mod.manifest" "%MOD_DEST%\"
    goto :done
)

echo ERREUR : mod "%MOD_NAME%" introuvable (ni C++ ni Lua).
exit /b 1

:done
echo [install] %MOD_NAME% installé dans %MOD_DEST%
