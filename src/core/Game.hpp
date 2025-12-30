// src/core/Game.hpp
#pragma once
#include "World.hpp"
#include "../levels/level1/Level1_Newton.hpp"
#include <raylib.h>

class Game {
public:
    void Init();
    void Update(float dt);
    void Render();
    void ResetGame();
    
    // ✅ Allow RenderSystem to access private members
    friend class RenderSystem;
    
private:
    World world;
    Level1_Newton currentLevel;
    Camera2D cam;
    bool levelLoaded = false;
    
    void DrawHUD();
    void DrawDebugInfo();
};