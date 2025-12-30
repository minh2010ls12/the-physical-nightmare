#pragma once
#include "Skill.hpp"
#include <raylib.h>
#include <raymath.h>

class SkillX_Shield : public Skill {
    float shieldTimer = 0;
    const float COOLDOWN = 15.0f;
    const float DURATION = 0.7f;

public:
    void Activate(World& w, size_t p) override {
        if (w.skill[p].cd_x < COOLDOWN) return;
        
        w.skill[p].cd_x = 0;
        shieldTimer = DURATION;
        w.health[p].invincible = true;
    }

    void Update(World& w, size_t p, float dt) override {
        w.skill[p].cd_x = std::min(COOLDOWN, w.skill[p].cd_x + 1.0f * dt);
        
        if (shieldTimer > 0) {
            shieldTimer -= dt;
            if (shieldTimer <= 0) {
                w.health[p].invincible = false;
            }
        }
    }

    void Render(Vector2 playerPos, float facing) override {
        if (shieldTimer > 0) {
            float pulse = sinf(GetTime() * 35) * 0.5f + 0.5f;
            float r = 95 + pulse * 35;
            
            DrawCircleLines(playerPos.x, playerPos.y, r + 25, ColorAlpha(SKYBLUE, 0.95f));
            DrawCircleLines(playerPos.x, playerPos.y, r + 18, ColorAlpha(YELLOW, 0.9f));
            
            DrawCircle(playerPos.x, playerPos.y, r, ColorAlpha((Color){0, 255, 255, 180}, pulse));
            DrawCircle(playerPos.x, playerPos.y, r - 20, ColorAlpha((Color){150, 255, 255, 120}, pulse));
            
            // Electric arcs
            for (int i = 0; i < 6; i++) {
                float angle = (GetTime() * 8 + i * 60) * DEG2RAD;
                float px = playerPos.x + cosf(angle) * r;
                float py = playerPos.y + sinf(angle) * r;
                DrawCircle(px, py, 8, ColorAlpha(YELLOW, pulse));
            }
            
            DrawText("REFLECT ACTIVE", playerPos.x - 130, playerPos.y - 160, 45, ColorAlpha(YELLOW, pulse));
            DrawText(TextFormat("%.1fs", shieldTimer), playerPos.x - 35, playerPos.y - 110, 55, ColorAlpha(WHITE, pulse));
        }
    }
    
    bool IsActive() const { return shieldTimer > 0; }
};