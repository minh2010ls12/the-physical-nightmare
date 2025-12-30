#include "CombatSystem.hpp"
#include "../skills/SkillZ_Plasma.hpp"
#include "../skills/SkillX_Shield.hpp"
#include "../skills/SkillC_Bomb.hpp"
#include "../skills/SkillV_Ultimate.hpp"
#include "../utills/Stubs.hpp"
#include <raylib.h>
#include <raymath.h>
#include <algorithm>
#include <iostream>

namespace CombatSystem {
    
    struct Slash { 
        Rectangle rect; 
        float timer; 
        Color color;
    };
    
    static std::vector<Slash> slashes;
    static bool playerDied = false;
    
    static bool IsEnemyEntity(const World& world, size_t idx) {
        return idx < world.ai.size() && world.ai[idx].type == 999;
    }

    void Init(World& world) {
        std::cout << "[COMBAT] Init called" << std::endl;
        
        // ✅ CRITICAL FIX: Don't touch skills here - Game.cpp handles it
        // The crash happens because we're deleting skills that Game.cpp is about to create
        
        slashes.clear();
        playerDied = false;
        
        std::cout << "[COMBAT] Init complete (skills managed by Game.cpp)" << std::endl;
    }

    void Update(World& world, float dt) {
        if (playerDied) {
            if (IsKeyPressed(KEY_ENTER)) {
                playerDied = false;
                slashes.clear();
                
                if (world.player != INVALID && world.index.count(world.player)) {
                    size_t p = world.index[world.player];
                    if (p < world.pos.size()) world.pos[p].pos = {640.0f, 500.0f};
                    if (p < world.health.size()) {
                        world.health[p].hp = 1000;
                        world.health[p].invincible = false;
                    }
                    if (p < world.input.size()) {
                        world.input[p].dashing = false;
                        world.input[p].dash_time = 0;
                    }
                }
            }
            return;
        }

        if (world.player == INVALID) return;
        
        auto playerIt = world.index.find(world.player);
        if (playerIt == world.index.end()) return;
        
        size_t p = playerIt->second;
        if (p >= world.pos.size()) return;
        
        Vector2 pos = world.pos[p].pos;
        float facing = world.pos[p].facing;
        auto& input = world.input[p];

        // Check player death
        if (p < world.health.size() && world.health[p].hp <= 0) { 
            playerDied = true; 
            ParticleSystem::EmitBlood(pos, 30, RED);
            return; 
        }

        // MELEE SLASH
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Rectangle slashRect = {
                pos.x + facing * 60 - (facing > 0 ? 0 : 120),
                pos.y - 35,
                facing * 120,
                70
            };
            
            slashes.push_back({slashRect, 0.15f, Color{255, 255, 255, 180}});
            SoundManager::Play("slash.wav");

            for (auto& [entityId, idx] : world.index) {
                if (entityId == world.player) continue;
                
                if (IsEnemyEntity(world, idx)) {
                    Rectangle eRect = {
                        world.pos[idx].pos.x - 24,
                        world.pos[idx].pos.y - 44,
                        48,
                        88
                    };
                    
                    if (CheckCollisionRecs(slashRect, eRect)) {
                        if (idx < world.health.size()) {
                            world.health[idx].hp -= 40;
                            ParticleSystem::EmitBlood(world.pos[idx].pos, 12, RED);
                            CameraShake(0.08f, 3.0f);
                        }
                    }
                }
            }
        }

        // DASH COMBAT BONUS
        if (p < world.input.size() && input.dashing) {
            if (p < world.health.size()) {
                world.health[p].invincible = true;
            }
            if (p < world.velocity.size()) {
                ParticleSystem::EmitTrail(pos, world.velocity[p].vel, 8, PURPLE);
            }
            
            Rectangle dashHitbox = {pos.x - 15, pos.y - 15, 30, 30};
            for (auto& [entityId, idx] : world.index) {
                if (entityId == world.player) continue;
                
                if (IsEnemyEntity(world, idx)) {
                    Rectangle eRect = {
                        world.pos[idx].pos.x - 24,
                        world.pos[idx].pos.y - 44,
                        48,
                        88
                    };
                    
                    if (CheckCollisionRecs(dashHitbox, eRect)) {
                        if (idx < world.health.size()) {
                            world.health[idx].hp -= 25;
                        }
                        if (idx < world.velocity.size()) {
                            world.velocity[idx].vel.x += facing * 200.0f;
                        }
                    }
                }
            }
        } else if (p < world.health.size()) {
            world.health[p].invincible = false;
        }

        // ✅ SKILL INPUTS - Add null checks
        if (IsKeyPressed(KEY_Z) && world.playerSkills[0]) {
            world.playerSkills[0]->Activate(world, p);
        }
        if (IsKeyPressed(KEY_X) && world.playerSkills[1]) {
            world.playerSkills[1]->Activate(world, p);
        }
        if (IsKeyPressed(KEY_C) && world.playerSkills[2]) {
            world.playerSkills[2]->Activate(world, p);
        }
        if (IsKeyPressed(KEY_V) && world.playerSkills[3]) {
            world.playerSkills[3]->Activate(world, p);
        }

        // UPDATE ALL SKILLS with null checks
        for (int i = 0; i < 4; i++) {
            if (world.playerSkills[i] != nullptr) {
                world.playerSkills[i]->Update(world, p, dt);
            }
        }

        // UPDATE SLASHES
        for (auto it = slashes.begin(); it != slashes.end();) {
            it->timer -= dt;
            if (it->timer <= 0) {
                it = slashes.erase(it);
            } else {
                ++it;
            }
        }
    }

    void Render(const World& world) {
        if (world.player == INVALID) return;
        
        auto playerIt = world.index.find(world.player);
        if (playerIt == world.index.end()) return;
        
        size_t p = playerIt->second;
        if (p >= world.pos.size()) return;
        
        Vector2 pos = world.pos[p].pos;
        float facing = world.pos[p].facing;

        // Render slashes
        for (const auto& s : slashes) {
            DrawRectangleRec(s.rect, s.color);
            DrawRectangleLinesEx(s.rect, 2, Color{255, 255, 0, (unsigned char)(s.timer * 500)});
        }

        // Render skills with null checks
        for (int i = 0; i < 4; i++) {
            if (world.playerSkills[i] != nullptr) {
                world.playerSkills[i]->Render(pos, facing);
            }
        }

        // Death screen
        if (playerDied) {
            DrawRectangle(0, 0, 1280, 720, {0, 0, 0, 200});
            DrawText("YOU DIED", 320, 240, 160, RED);
            
            float blink = (sinf(GetTime() * 6) + 1) * 0.5f;
            DrawText("PRESS ENTER", 420, 400, 60, Fade(WHITE, blink));
            DrawText("Respawn at (640, 500)", 450, 480, 30, GRAY);
        }
    }
    
} // namespace CombatSystem