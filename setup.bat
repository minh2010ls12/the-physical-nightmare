@echo off
echo.
echo ================================================
echo   BUILDING THE PHYSICAL NIGHTMARE - KHONG CAN CMAKE
echo ================================================
echo.

if not exist build mkdir build

g++ src/*.cpp src/core/*.cpp src/systems/*.cpp ^
    -o build/ThePhysicalNightmare.exe ^
    -I"E:/AppData/raylib/raylib/src" ^
    -L"E:/AppData/raylib/raylib/src" ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -static -static-libgcc -static-libstdc++ ^
    -O3

if %errorlevel% == 0 (
    echo.
    echo ================================================
    echo   BUILD THANH CONG 100% - CHAY DI BRO!!!
    echo ================================================
    build\ThePhysicalNightmare.exe
) else (
    echo.
    echo BUILD THAT BAI ROI BRO... NHUNG MA VAN CHAY DUOC MA 
    pause
)a