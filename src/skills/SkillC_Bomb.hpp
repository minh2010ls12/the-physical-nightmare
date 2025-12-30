#pragma once
#include "Skill.hpp"
#include <vector>
#include <raylib.h>
#include <raymath.h>
struct Bomb {
    Vector2 pos;
    Vector2 vel;
    float life;
};

class SkillC_Bomb : public Skill {
    std::vector<Bomb> bombs;
    float flashTimer = 0;
    const float COOLDOWN = 15.0f;
    const int DAMAGE = 15;
    const float GRAVITY = 1400.0f;

public:
    void Activate(World& w, size_t p) override {
        if (w.skill[p].cd_c < COOLDOWN) return;
        
        w.skill[p].cd_c = 0;
        Vector2 playerPos = w.pos[p].pos;
        float facing = w.pos[p].facing;
        
        // Spawn 6 bombs with spread
        for (int i = 0; i < 6; i++) {
            float spreadAngle = (i - 2.5f) * 15.0f;
            float angle = 30.0f + spreadAngle; // 30° base angle
            float rad = angle * DEG2RAD;
            
            Vector2 vel = {cosf(rad) * 900 * facing, -sinf(rad) * 900}; // Negative Y = up
            Vector2 spawnPos = {playerPos.x + facing * 60, playerPos.y - 60};
            
            bombs.push_back({spawnPos, vel, 2.0f});
        }
        
        flashTimer = 0.2f;
    }

    void Update(World& w, size_t p, float dt) override {
        w.skill[p].cd_c = std::min(COOLDOWN, w.skill[p].cd_c + 1.0f * dt);
        
        if (flashTimer > 0) flashTimer -= dt;
        
        for (auto it = bombs.begin(); it != bombs.end();) {
            it->pos.x += it->vel.x * dt;
            it->pos.y += it->vel.y * dt;
            it->vel.y += GRAVITY * dt; // Apply gravity
            it->life -= dt;
            
            bool remove = (it->life <= 0 || it->pos.y > 800);
            
            // Check collision with enemies
            for (auto& pair : w.index) {
                size_t i = pair.second;
                if (i >= w.ai.size() || w.ai[i].type != 999) continue;
                
                float dist = Vector2Distance(it->pos, w.pos[i].pos);
                if (dist < 60) { // Larger hitbox for bombs
                    w.health[i].hp -= DAMAGE;
                    remove = true;
                    break;
                }
            }
            
            if (remove) it = bombs.erase(it);
            else ++it;
        }
    }

    void Render(Vector2 playerPos, float facing) override {
        if (flashTimer > 0) {
            float r = 70 + flashTimer * 500;
            DrawCircle(playerPos.x + facing * 60, playerPos.y - 60, r, ColorAlpha(ORANGE, flashTimer * 4));
            DrawCircleLines(playerPos.x + facing * 60, playerPos.y - 60, r + 30, ColorAlpha(YELLOW, flashTimer * 3));
        }
        
        for (const auto& b : bombs) {
            // Trail
            for (int i = 3; i >= 1; i--) {
                Vector2 t = {b.pos.x - b.vel.x * 0.02f * i, b.pos.y - b.vel.y * 0.02f * i};
                DrawCircle(t.x, t.y, 25 - i * 6, ColorAlpha(ORANGE, 0.7f - i * 0.15f));
            }
            DrawCircle(b.pos.x, b.pos.y, 25, ORANGE);
            DrawCircle(b.pos.x, b.pos.y, 18, YELLOW);
            DrawCircle(b.pos.x, b.pos.y, 8, WHITE);
        }
    }
};