@echo off
setlocal enabledelayedexpansion
title Pobieranie Zaleznosci NPU

echo ==========================================================
echo    NPU Model Loader - Instalator Bibliotek
echo ==========================================================
echo.
echo Pobieranie bibliotek C++ (ONNX Runtime, Vitis AI, ok. 1 GB)...

:: Zmień poniższy link na link do pliku "deps.zip", 
:: który wrzucisz w zakładce "Releases" na swoim własnym GitHubie!
set "DEPS_URL=https://github.com/TwojNick/NPU-Model-Loader/releases/download/v1.0/deps.zip"
set "TARGET_DIR=%~dp0npu_chat"

if not exist "%TARGET_DIR%\deps.zip" (
    echo [1/2] Pobieranie pakietu zaleznosci...
    curl -L -o "%TARGET_DIR%\deps.zip" "%DEPS_URL%"
) else (
    echo [1/2] Plik deps.zip juz istnieje.
)

echo [2/2] Rozpakowywanie archiwum...
powershell -Command "Expand-Archive -Path '%TARGET_DIR%\deps.zip' -DestinationPath '%TARGET_DIR%' -Force"

echo Czyszczenie archiwum...
del "%TARGET_DIR%\deps.zip"

echo.
echo Gotowe! Biblioteki "libs" oraz "include" zostaly zainstalowane.
echo Mozesz teraz skompilowac projekt.
pause
