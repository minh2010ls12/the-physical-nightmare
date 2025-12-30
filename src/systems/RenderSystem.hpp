// src/systems/RenderSystem.hpp
#pragma once
#include "../core/World.hpp"
#include "../levels/level1/Level1_Newton.hpp"
#include <raylib.h>
#include <raymath.h>

class RenderSystem {
public:
    static void Init();
    static void Cleanup();
    static void ResizeBuffers();
    
    // ✅ FIX #5: Correct signature with Level1_Newton
    static void RenderAll(World& world, const Camera2D& cam, Level1_Newton& level);
    
    // Render layers
    static void RenderBackground();
    static void RenderTerrain(const Rectangle& bounds);  // ✅ FIX #3: Correct signature
    static void RenderEntities(World& world);
    static void RenderPlayer(World& world);
    static void RenderSkillEffects(World& world);
    static void RenderUI(World& world);
    static void RenderDebug(World& world, const Camera2D& cam);
    
    // Helpers
    static void DrawPlatform(const Rectangle& rect, float friction);
    static void DrawEnemy(const Vector2& pos, const Color& color, int hp, int maxHp);
    static void DrawHealthBar(const Vector2& pos, int hp, int maxHp, float width);
    
private:
    static Shader backgroundShader;
    static Shader fogShader;
    static Shader postProcessShader;
    static RenderTexture2D lightingBuffer;
    static RenderTexture2D postBuffer;
    static bool initialized;
};