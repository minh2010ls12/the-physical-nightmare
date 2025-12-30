#pragma once
#include "Skill.hpp"
#include <vector>
#include <raylib.h>
#include <raymath.h>

struct Sword {
    Vector2 pos;
    float life;
    Entity targetEnemy = INVALID;
};

class SkillV_Ultimate : public Skill {
    std::vector<Sword> swords;
    float flashTimer = 0;
    const float COOLDOWN = 120.0f;
    const int DAMAGE_PER_SWORD = 5;
    const float SWORD_SPEED = 2000.0f;

public:
    void Activate(World& w, size_t p) override {
        if (w.skill[p].cd_v < COOLDOWN) return;
        
        w.skill[p].cd_v = 0;
        
        // Self damage 10%
        w.health[p].hp = w.health[p].hp * 90 / 100;
        
        flashTimer = 0.5f;
        
        Vector2 playerPos = w.pos[p].pos;
        int swordCount = 10 + (rand() % 11); // 10-20
        
        for (int k = 0; k < swordCount; k++) {
            float offsetY = (k - swordCount/2.0f) * 15.0f;
            float offsetX = ((k % 4) - 2) * 30.0f;
            Vector2 spawn = {playerPos.x + offsetX, playerPos.y + offsetY - 600};
            
            swords.push_back({spawn, 4.0f, INVALID});
        }
    }

    void Update(World& w, size_t p, float dt) override {
        w.skill[p].cd_v = std::min(COOLDOWN, w.skill[p].cd_v + 1.0f * dt);
        
        if (flashTimer > 0) flashTimer -= dt;
        
        for (auto it = swords.begin(); it != swords.end();) {
            it->life -= dt;
            
            // Find nearest enemy to track
            float bestDist = 99999;
            Entity bestEnemy = INVALID;
            
            for (auto& pair : w.index) {
                size_t i = pair.second;
                if (i >= w.ai.size() || w.ai[i].type != 999) continue;
                
                float dist = Vector2Distance(it->pos, w.pos[i].pos);
                if (dist < bestDist) {
                    bestDist = dist;
                    bestEnemy = pair.first;
                }
            }
            
            // Move towards target
            Vector2 target;
            if (bestEnemy != INVALID && w.index.count(bestEnemy)) {
                size_t idx = w.index[bestEnemy];
                target = w.pos[idx].pos;
            } else {
                // No enemy - fall down
                target = {it->pos.x, it->pos.y + 3000};
            }
            
            Vector2 dir = Vector2Normalize(Vector2Subtract(target, it->pos));
            it->pos.x += dir.x * SWORD_SPEED * dt;
            it->pos.y += dir.y * SWORD_SPEED * dt;
            
            bool remove = (it->life <= 0 || it->pos.y > 800);
            
            // Check collision
            for (auto& pair : w.index) {
                size_t i = pair.second;
                if (i >= w.ai.size() || w.ai[i].type != 999) continue;
                
                float dist = Vector2Distance(it->pos, w.pos[i].pos);
                if (dist < 40) {
                    w.health[i].hp -= DAMAGE_PER_SWORD;
                    remove = true;
                    break;
                }
            }
            
            if (remove) it = swords.erase(it);
            else ++it;
        }
    }

    void Render(Vector2 playerPos, float facing) override {
        if (flashTimer > 0) {
            DrawRectangle(0, 0, 1280, 720, Color{120, 0, 200, (unsigned char)(flashTimer * 500)});
            float alpha = fminf(flashTimer * 3.0f, 1.0f);
            DrawText("NGUYET KIEM HAC VU!!!", 140, 280, 120, Fade(MAGENTA, alpha));
            if (fmodf(GetTime() * 12, 2) < 1) {
                DrawText(">>> ULTIMATE <<<", 380, 380, 80, Fade(WHITE, alpha));
            }
        }
        
        for (const auto& s : swords) {
            // Sword visual
            DrawCircle(s.pos.x, s.pos.y, 20, MAGENTA);
            DrawCircle(s.pos.x, s.pos.y, 12, WHITE);
            DrawCircle(s.pos.x, s.pos.y, 5, YELLOW);
            
            // Trail
            DrawCircle(s.pos.x, s.pos.y + 15, 8, ColorAlpha(MAGENTA, 0.5f));
            DrawCircle(s.pos.x, s.pos.y + 25, 5, ColorAlpha(MAGENTA, 0.3f));
        }
    }
};