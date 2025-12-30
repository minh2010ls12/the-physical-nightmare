#include "Level1_Zone3_Fall.hpp"
#include "../../utills/PhysicsSystem.hpp"
#include "../../utills/Stubs.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

void Level1_Zone3_Fall::Load(World& world) {
    std::cout << "[ZONE 3] Loading Falling Zone..." << std::endl;
    
    bounds = {0, 0, 5000, 5000};
    fallingRocks.clear();
    
    // Create falling rocks - sử dụng Rectangle constructor đúng
    fallingRocks.push_back(Rectangle{1500, 160, 50, 20});
    fallingRocks.push_back(Rectangle{1620, 110, 50, 20});
    fallingRocks.push_back(Rectangle{1740, 60, 50, 20});
    fallingRocks.push_back(Rectangle{1580, 210, 50, 20});
    fallingRocks.push_back(Rectangle{1700, 130, 50, 20});
    
    // Add wind zone that pushes upward
    PhysicsSystem::AddWindZone({bounds.x + bounds.width/2, bounds.y + 100}, 
                              200.0f, 0.0f, -30.0f);
}

void Level1_Zone3_Fall::Update(World& world, float dt) {
    if (world.player == INVALID) return;
    
    auto playerIt = world.index.find(world.player);
    if (playerIt == world.index.end()) return;
    
    size_t p = playerIt->second;
    
    if (p >= world.pos.size() || p >= world.velocity.size()) return;
    
    Vector2 playerPos = world.pos[p].pos;
    
    // Check if player is in this zone
    if (playerPos.x >= bounds.x && playerPos.x <= bounds.x + bounds.width &&
        playerPos.y >= bounds.y && playerPos.y <= bounds.y + bounds.height) {
        
        // Apply upward wind force
        world.velocity[p].vel.y -= 110.0f * dt; // 1.1f * 100
        
        // Limit fall speed
        if (world.velocity[p].vel.y > 300.0f) {
            world.velocity[p].vel.y = 300.0f;
        }
    }
    
    // Update falling rocks
    for (auto& rock : fallingRocks) {
        // Apply gravity
        rock.y += 100.0f * dt;
        
        // Reset if fallen
        if (rock.y > bounds.y + bounds.height + 50) {
            rock.y = bounds.y - 50;
            rock.x = bounds.x + GetRandomValue(50, (int)bounds.width - 100);
        }
        
        // Check collision with player
        if (p < world.collider.size()) {
            Rectangle playerRect = {
                world.pos[p].pos.x - world.collider[p].rect.width/2,
                world.pos[p].pos.y - world.collider[p].rect.height/2,
                world.collider[p].rect.width,
                world.collider[p].rect.height
            };
            
            if (CheckCollisionRecs(rock, playerRect)) {
                // Damage player
                if (p < world.health.size()) {
                    world.health[p].hp -= 10;
                    
                    // Knockback
                    world.velocity[p].vel.x += (playerRect.x < rock.x) ? -200.0f : 200.0f;
                    world.velocity[p].vel.y = -150.0f;
                    
                    // Sound effect
                    SoundManager::Play("rock_hit.wav");
                    
                    // Reset rock
                    rock.y = bounds.y - 100;
                }
            }
        }
    }
}

void Level1_Zone3_Fall::Render() {
    // Background
    DrawRectangleGradientV(
        (int)bounds.x, (int)bounds.y,
        (int)bounds.width, (int)bounds.height,
        (Color){50, 40, 60, 255},
        (Color){70, 60, 80, 255}
    );
    
    // Falling rocks
    for (const auto& rock : fallingRocks) {
        // Rock body
        DrawRectangleRec(rock, (Color){120, 110, 100, 255});
        DrawRectangleLinesEx(rock, 2.0f, DARKGRAY);
        
        // Rock detail
        DrawRectangle(rock.x + 5, rock.y + 5, rock.width - 10, rock.height - 10,
                     (Color){100, 90, 80, 255});
        
        // Falling trail
        for (int i = 1; i <= 3; i++) {
            float alpha = 1.0f - (float)i / 4.0f;
            DrawRectangle(rock.x + 10, rock.y - i * 8, rock.width - 20, 4,
                         ColorAlpha(GRAY, alpha * 0.5f));
        }
    }
    
    // Wind visualization
    for (int i = 0; i < 10; i++) {
        float x = bounds.x + fmod(GetTime() * 40 + i * 45, bounds.width);
        for (int j = 0; j < 6; j++) {
            float y = bounds.y + j * 40;
            
            DrawCircle(x, y, 2.0f, ColorAlpha(SKYBLUE, 0.6f));
            DrawLineEx({x, y}, {x, y - 15}, 1.5f, ColorAlpha(SKYBLUE, 0.4f));
        }
    }
    
    // Zone label
    DrawText("VUNG ROI TU DO",
            (int)(bounds.x + bounds.width/2 - 70),
            (int)bounds.y + 20,
            22, (Color){180, 160, 140, 255});
    
    // Warning
    float pulse = (sinf(GetTime() * 3) + 1) * 0.5f;
    DrawText("WARNING: Reduced gravity!",
            (int)(bounds.x + bounds.width/2 - 120),
            (int)bounds.y + 50,
            18, ColorAlpha(RED, pulse));
}

void Level1_Zone3_Fall::Unload(World& world) {
    fallingRocks.clear();
}