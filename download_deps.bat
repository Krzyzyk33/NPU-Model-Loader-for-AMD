@echo off
setlocal enabledelayedexpansion
title NPU Dependencies Downloader

echo ==========================================================
echo    NPU Model Loader - Dependencies Installer
echo ==========================================================
echo.
echo Downloading C++ libraries (ONNX Runtime, Vitis AI, approx 1 GB)...

:: Change the link below to point to the "deps.zip" file
set "DEPS_URL=https://github.com/Krzyzyk33/NPU-Model-Loader-for-AMD/releases/download/v1.0/deps.zip"
set "TARGET_DIR=%~dp0npu_chat"

if not exist "%TARGET_DIR%\deps.zip" (
    echo [1/2] Downloading dependencies package...
    curl -L -o "%TARGET_DIR%\deps.zip" "%DEPS_URL%"
) else (
    echo [1/2] deps.zip already exists.
)

echo [2/2] Extracting archive...
powershell -Command "Expand-Archive -Path '%TARGET_DIR%\deps.zip' -DestinationPath '%TARGET_DIR%' -Force"

echo Cleaning up archive...
del "%TARGET_DIR%\deps.zip"

echo.
echo Done! The "libs" and "include" folders have been installed.
echo You can now compile the project.
pause
