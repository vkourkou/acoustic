@echo off
echo Building AcousticProject with CUDA support...
rmdir /s /q build
mkdir build
cd build
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd ..

echo.
echo ========================================
echo Build successful!
echo ========================================
echo.
echo Main executable location:
if exist "build\Release\bin\acoustic.exe" (
    echo   build\Release\bin\acoustic.exe
) else if exist "build\acoustic.exe" (
    echo   build\acoustic.exe
) else (
    echo   (executable not found in expected locations)
)
echo.
echo Unit test executable location:
if exist "build\Release\bin\ut.exe" (
    echo   build\Release\bin\ut.exe
) else if exist "build\ut.exe" (
    echo   build\ut.exe
) else (
    echo   (unit test executable not found in expected locations)
)
echo.

