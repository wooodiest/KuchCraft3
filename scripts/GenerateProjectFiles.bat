@echo off

set PREMAKE_ARGS=vs2022

if not "%1"=="" set PREMAKE_ARGS=%1

pushd %~dp0\..\

if not exist premake\premake5.exe (
    echo ERROR: premake5.exe not found in premake\ folder!
    pause
    popd
    exit /b 1
)

call premake\premake5.exe %PREMAKE_ARGS%

if errorlevel 1 (
    echo Premake failed!
    pause
    popd
    exit /b 1
)

popd
PAUSE
