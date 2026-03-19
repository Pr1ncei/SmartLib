@echo off
echo Select Visual Studio version:
echo 1. VS 2019
echo 2. VS 2022
echo 3. VS 2026
set /p choice=Enter choice (1-3): 

if "%choice%"=="1" (
    echo Generating VS 2019 solution...
    premake5 vs2019
) else if "%choice%"=="2" (
    echo Generating VS 2022 solution...
    premake5 vs2022
) else if "%choice%"=="3" (
    echo Generating VS 2026 solution...
    premake5 vs2026
) else (
    echo Invalid choice!
    pause
    exit /b
)

echo Done!
echo Open Opac.sln
pause