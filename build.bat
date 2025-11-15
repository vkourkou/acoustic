@echo off
REM Windows Batch Build Script for C++ Project
REM This script provides a simple way to build the project on Windows

setlocal enabledelayedexpansion

REM Configuration
set BUILD_TYPE=Release
set COMPILER=cl
set CMAKE_PATH=
set GENERATOR=

REM Parse command line arguments
:parse_args
if "%1"=="" goto :end_parse
if /i "%1"=="debug" set BUILD_TYPE=Debug
if /i "%1"=="release" set BUILD_TYPE=Release
if /i "%1"=="clean" goto :clean
if /i "%1"=="rebuild" goto :rebuild
if /i "%1"=="help" goto :help
if /i "%1"=="vs2019" set GENERATOR=-G "Visual Studio 16 2019"
if /i "%1"=="vs2022" set GENERATOR=-G "Visual Studio 17 2022"
if "%1"=="--cmake" set CMAKE_PATH=%2
shift
goto :parse_args
:end_parse

REM Check for CMake
if "%CMAKE_PATH%"=="" (
    where cmake >nul 2>&1
    if errorlevel 1 (
        echo ERROR: CMake not found in PATH.
        echo Please install CMake or specify path with --cmake option.
        echo Download from: https://cmake.org/download/
        exit /b 1
    )
    set CMAKE_PATH=cmake
)

REM Check for Visual Studio compiler
where cl >nul 2>&1
if errorlevel 1 (
    echo WARNING: Visual Studio compiler not found in PATH.
    echo Attempting to find Visual Studio installation...
    if exist "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    ) else (
        echo ERROR: Could not find Visual Studio installation.
        echo Please install Visual Studio or run from Developer Command Prompt.
        exit /b 1
    )
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring project with CMake...
if "%GENERATOR%"=="" (
    %CMAKE_PATH% .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
) else (
    %CMAKE_PATH% .. %GENERATOR% -A x64
)

if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    exit /b 1
)

REM Build the project
echo Building project...
if "%GENERATOR%"=="" (
    %CMAKE_PATH% --build . --config %BUILD_TYPE%
) else (
    %CMAKE_PATH% --build . --config %BUILD_TYPE%
)

if errorlevel 1 (
    echo ERROR: Build failed.
    exit /b 1
)

echo.
echo Build successful!
echo Executable location: build\%BUILD_TYPE%\bin\acoustic.exe
echo Unit test executable location: build\%BUILD_TYPE%\bin\ut.exe
cd ..
goto :end

:clean
echo Cleaning build artifacts...
if exist "build" (
    rmdir /s /q build
    echo Clean complete.
) else (
    echo Nothing to clean.
)
goto :end

:rebuild
call %0 clean
call %0 %*
goto :end

:help
echo.
echo Build Script Usage:
echo   build.bat [options] [target]
echo.
echo Options:
echo   debug          - Build in Debug mode (default: Release)
echo   release        - Build in Release mode
echo   clean          - Remove all build artifacts
echo   rebuild        - Clean and rebuild
echo   vs2019         - Use Visual Studio 2019 generator
echo   vs2022         - Use Visual Studio 2022 generator
echo   --cmake PATH   - Specify CMake executable path
echo   help           - Show this help message
echo.
echo Examples:
echo   build.bat
echo   build.bat debug
echo   build.bat vs2022
echo   build.bat clean
goto :end

:end
endlocal

