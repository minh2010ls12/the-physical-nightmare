#pragma once
#include "Skill.hpp"
#include <vector>
#include <raylib.h>
#include <raymath.h>

struct PlasmaProjectile {
    Vector2 pos;
    Vector2 vel;
    float life;
};

class SkillZ_Plasma : public Skill {
    std::vector<PlasmaProjectile> projectiles;
    float flashTimer = 0;
    const float COOLDOWN = 5.0f;
    const int DAMAGE = 13;

public:
    void Activate(World& w, size_t p) override {
        if (w.skill[p].cd_z < COOLDOWN) return;
        
        w.skill[p].cd_z = 0;
        Vector2 playerPos = w.pos[p].pos;
        float facing = w.pos[p].facing;
        
        // Spawn at player position + offset
        Vector2 spawnPos = {playerPos.x + facing * 40, playerPos.y};
        Vector2 vel = {facing * 1500, 0};
        
        projectiles.push_back({spawnPos, vel, 3.5f});
        flashTimer = 0.15f;
    }

    void Update(World& w, size_t p, float dt) override {
        // Cooldown recovery
        w.skill[p].cd_z = std::min(COOLDOWN, w.skill[p].cd_z + 1.0f * dt);
        
        // Flash timer
        if (flashTimer > 0) flashTimer -= dt;
        
        // Update projectiles
        for (auto it = projectiles.begin(); it != projectiles.end();) {
            it->pos.x += it->vel.x * dt;
            it->pos.y += it->vel.y * dt;
            it->life -= dt;
            
            bool remove = (it->life <= 0 || it->pos.x < -100 || it->pos.x > 1400);
            
            // Check collision with enemies
            for (auto& pair : w.index) {
                size_t i = pair.second;
                if (i >= w.ai.size() || w.ai[i].type != 999) continue;
                
                float dist = Vector2Distance(it->pos, w.pos[i].pos);
                if (dist < 55) {
                    w.health[i].hp -= DAMAGE;
                    remove = true;
                    break;
                }
            }
            
            if (remove) it = projectiles.erase(it);
            else ++it;
        }
    }

    void Render(Vector2 playerPos, float facing) override {
        // Flash effect
        if (flashTimer > 0) {
            float r = 60 + flashTimer * 400;
            DrawCircle(playerPos.x + facing * 40, playerPos.y, r, ColorAlpha(MAGENTA, flashTimer * 5));
            DrawCircleLines(playerPos.x + facing * 40, playerPos.y, r + 20, ColorAlpha(WHITE, flashTimer * 4));
        }
        
        // Projectiles
        for (const auto& p : projectiles) {
            // Trail
            for (int i = 3; i >= 1; i--) {
                Vector2 t = {p.pos.x - p.vel.x * 0.02f * i, p.pos.y - p.vel.y * 0.02f * i};
                DrawCircle(t.x, t.y, 30 - i * 7, ColorAlpha(MAGENTA, 0.8f - i * 0.2f));
            }
            DrawCircle(p.pos.x, p.pos.y, 28, MAGENTA);
            DrawCircle(p.pos.x, p.pos.y, 20, WHITE);
            DrawCircle(p.pos.x, p.pos.y, 10, YELLOW);
        }
    }
};