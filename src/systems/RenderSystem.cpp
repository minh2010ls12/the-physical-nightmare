// src/systems/RenderSystem.cpp
#include "RenderSystem.hpp"
#include "../utills/PhysicsSystem.hpp"
#include "../skills/Skill.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

Shader RenderSystem::backgroundShader = {0};
Shader RenderSystem::fogShader = {0};
Shader RenderSystem::postProcessShader = {0};
RenderTexture2D RenderSystem::lightingBuffer = {0};
RenderTexture2D RenderSystem::postBuffer = {0};
bool RenderSystem::initialized = false;

void RenderSystem::Init() {
    if (initialized) {
        std::cout << "[RENDER] Already initialized" << std::endl;
        return;
    }
    
    std::cout << "[RENDER] Initializing..." << std::endl;
    ResizeBuffers();
    initialized = true;
    std::cout << "[RENDER] Initialized successfully" << std::endl;
}

void RenderSystem::ResizeBuffers() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    
    if (lightingBuffer.texture.id > 0) UnloadRenderTexture(lightingBuffer);
    if (postBuffer.texture.id > 0) UnloadRenderTexture(postBuffer);
    
    lightingBuffer = LoadRenderTexture(w, h);
    postBuffer = LoadRenderTexture(w, h);
}

void RenderSystem::Cleanup() {
    if (!initialized) return;
    
    if (backgroundShader.id > 0) UnloadShader(backgroundShader);
    if (fogShader.id > 0) UnloadShader(fogShader);
    if (postProcessShader.id > 0) UnloadShader(postProcessShader);
    if (lightingBuffer.texture.id > 0) UnloadRenderTexture(lightingBuffer);
    if (postBuffer.texture.id > 0) UnloadRenderTexture(postBuffer);
    
    initialized = false;
}

void RenderSystem::RenderBackground() {
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(),
                           (Color){20, 10, 30, 255}, (Color){5, 0, 10, 255});
    
    for (int i = 0; i < 50; i++) {
        float x = (float)(i * 123 % GetScreenWidth());
        float y = (float)(i * 456 % GetScreenHeight());
        float alpha = 0.5f + sinf(GetTime() + i) * 0.3f;
        DrawPixel(x, y, ColorAlpha(WHITE, alpha));
    }
}

void RenderSystem::RenderTerrain(const Rectangle& bounds) {
    DrawRectangleRec(bounds, (Color){50, 30, 60, 255});
    DrawRectangleLinesEx(bounds, 2.0f, (Color){100, 50, 120, 255});
    
    for (int i = 0; i < 5; i++) {
        float x1 = bounds.x + (float)i * bounds.width / 5;
        float x2 = x1 + GetRandomValue(20, 50);
        DrawLineEx({x1, bounds.y + bounds.height}, 
                   {x2, bounds.y + bounds.height - 10},
                   1.5f, (Color){30, 15, 40, 255});
    }
}

void RenderSystem::RenderEntities(World& world) {
    std::vector<std::pair<size_t, float>> entityOrder;
    
    for (const auto& pair : world.index) {
        Entity e = pair.first;
        size_t idx = pair.second;
        if (idx < world.pos.size() && idx < world.sprite.size()) {
            entityOrder.emplace_back(idx, world.pos[idx].pos.y);
        }
    }
    
    std::sort(entityOrder.begin(), entityOrder.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    BeginBlendMode(BLEND_ALPHA);
    for (const auto& [idx, y] : entityOrder) {
        if (idx >= world.pos.size() || idx >= world.sprite.size()) continue;
        
        Vector2 pos = world.pos[idx].pos;
        Color color = world.sprite[idx].color;
        
        DrawEllipse(pos.x + 5, pos.y + 15, 10, 5, ColorAlpha(BLACK, 0.4f));
        DrawRectangle(pos.x - 8, pos.y - 12, 16, 24, color);
        
        if (idx < world.health.size() && world.health[idx].hp < world.health[idx].max_hp / 2) {
            float flicker = sinf(GetTime() * 20 + pos.x) * 0.2f + 0.8f;
            DrawRectangle(pos.x - 8, pos.y - 12, 16, 24, ColorAlpha(RED, flicker));
        }
    }
    EndBlendMode();
}

void RenderSystem::RenderPlayer(World& world) {
    if (world.player == INVALID) return;
    auto it = world.index.find(world.player);
    if (it == world.index.end()) return;
    
    size_t p = it->second;
    if (p >= world.pos.size() || p >= world.sprite.size()) return;
    
    Vector2 pos = world.pos[p].pos;
    float facing = world.pos[p].facing;
    Color color = world.sprite[p].color;
    
    DrawEllipse(pos.x + 5 * facing, pos.y + 15, 12 + sinf(GetTime() * 5) * 2, 6, 
                ColorAlpha(BLACK, 0.5f));
    
    DrawRectangle(pos.x - 8, pos.y - 12, 16, 24, color);
    DrawCircle(pos.x, pos.y - 8, 8, color);
    
    if (p < world.input.size() && world.input[p].dashing) {
        for (int i = 1; i <= 5; i++) {
            Vector2 trailPos = {pos.x - facing * 10 * i, pos.y};
            DrawRectangle(trailPos.x - 8, trailPos.y - 12, 16, 24,
                         ColorAlpha(color, 0.6f - i * 0.1f));
        }
    }
}

void RenderSystem::RenderSkillEffects(World& world) {
    if (world.player == INVALID) return;
    auto it = world.index.find(world.player);
    if (it == world.index.end()) return;
    
    size_t p = it->second;
    Vector2 pos = world.pos[p].pos;
    float facing = world.pos[p].facing;
    
    for (int i = 0; i < 4; i++) {
        if (world.playerSkills[i]) {
            world.playerSkills[i]->Render(pos, facing);
        }
    }
    
    BeginBlendMode(BLEND_ADDITIVE);
    DrawCircle(pos.x, pos.y, 50, ColorAlpha(PURPLE, 0.2f));
    EndBlendMode();
}

void RenderSystem::RenderUI(World& world) {
    if (world.player == INVALID) return;
    auto it = world.index.find(world.player);
    if (it == world.index.end()) return;
    
    size_t p = it->second;
    
    if (p < world.health.size()) {
        int hp = world.health[p].hp;
        int maxHp = world.health[p].max_hp;
        DrawRectangle(20, 20, 200, 20, BLACK);
        DrawRectangle(20, 20, (float)hp / maxHp * 200, 20, RED);
        DrawText(TextFormat("HP: %d/%d", hp, maxHp), 25, 25, 15, WHITE);
    }
    
    if (p < world.infected.size()) {
        int mana = world.infected[p].phsp;
        int maxMana = world.infected[p].max_phsp;
        DrawRectangle(20, 50, 200, 20, BLACK);
        DrawRectangle(20, 50, (float)mana / maxMana * 200, 20, BLUE);
        DrawText(TextFormat("MANA: %d/%d", mana, maxMana), 25, 55, 15, WHITE);
    }
    
    if (p < world.skill.size()) {
        const char* skills[] = {"Z", "X", "C", "V"};
        float cds[] = {world.skill[p].cd_z, world.skill[p].cd_x,
                       world.skill[p].cd_c, world.skill[p].cd_v};
        float maxCds[] = {5.0f, 15.0f, 15.0f, 120.0f};
        
        for (int i = 0; i < 4; i++) {
            int x = 250 + i * 60;
            DrawRectangle(x, 20, 50, 50, BLACK);
            float fill = cds[i] / maxCds[i];
            DrawRectangle(x, 20 + (1 - fill) * 50, 50, fill * 50, GRAY);
            DrawText(skills[i], x + 20, 35, 20, WHITE);
        }
    }
}

void RenderSystem::RenderDebug(World& world, const Camera2D& cam) {
    if (!IsKeyDown(KEY_F3)) return;
    
    DrawText(TextFormat("Entities: %zu", world.GetEntityCount()), 10, 30, 20, YELLOW);
    
    if (world.player != INVALID) {
        auto it = world.index.find(world.player);
        if (it != world.index.end()) {
            size_t p = it->second;
            if (p < world.pos.size()) {
                Vector2 pos = world.pos[p].pos;
                DrawText(TextFormat("Player: (%.0f, %.0f)", pos.x, pos.y), 10, 50, 20, GREEN);
            }
        }
    }
    
    PhysicsSystem::RenderDebug();
}

void RenderSystem::DrawPlatform(const Rectangle& rect, float friction) {
    Color color = (friction < 1.0f) ? RED : (friction > 1.0f) ? GREEN : GRAY;
    DrawRectangleRec(rect, color);
    DrawText(TextFormat("%.1f", friction), rect.x + 5, rect.y - 15, 10, WHITE);
}

void RenderSystem::DrawEnemy(const Vector2& pos, const Color& color, int hp, int maxHp) {
    DrawRectangle(pos.x - 10, pos.y - 15, 20, 30, color);
    DrawHealthBar({pos.x - 15, pos.y - 20}, hp, maxHp, 30.0f);
}

void RenderSystem::DrawHealthBar(const Vector2& pos, int hp, int maxHp, float width) {
    DrawRectangle(pos.x, pos.y, width, 5, BLACK);
    DrawRectangle(pos.x, pos.y, (float)hp / maxHp * width, 5, RED);
}

// ✅ MAIN RENDER PIPELINE WITH ZONE RENDERING
void RenderSystem::RenderAll(World& world, const Camera2D& cam, Level1_Newton& level) {
    static int lastW = 0, lastH = 0;
    int w = GetScreenWidth(), h = GetScreenHeight();
    if (w != lastW || h != lastH) {
        ResizeBuffers();
        lastW = w; lastH = h;
    }
    
    RenderBackground();
    
    BeginMode2D(cam);
    
    // ✅ CRITICAL: Let level render zones (handles zone switching)
    level.Render();
    
    RenderEntities(world);
    RenderPlayer(world);
    RenderSkillEffects(world);
    
    EndMode2D();
    
    RenderUI(world);
    RenderDebug(world, cam);
}