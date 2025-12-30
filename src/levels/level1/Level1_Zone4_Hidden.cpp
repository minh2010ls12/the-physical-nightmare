#include "Level1_Zone4_Hidden.hpp"
#include "../../core/World.hpp"
#include "../../utills/Stubs.hpp"
#include <iostream>

void Level1_Zone4_Hidden::Load(World& world) {
    std::cout << "[ZONE 4] Loading Hidden Zone..." << std::endl;
    
    bounds = {1920, 0, 180, 240};
    
    skillChests.clear();
    skillChests.push_back({{1980, 180, 50, 50}, 0, false});
    
    // ✅ FIX: Sync dog position from World
    if (Vector2Equals(world.dogPosition, {0, 0})) {
        world.dogPosition.x = bounds.x + GetRandomValue(10, (int)bounds.width - 30);
        world.dogPosition.y = bounds.y + bounds.height - GetRandomValue(40, 60);
        std::cout << "[ZONE 4] Dog randomized at: (" 
                  << world.dogPosition.x << ", " << world.dogPosition.y << ")" << std::endl;
    }
    dogPosition = world.dogPosition;  // Copy to local
    
    // ✅ FIX: Sync fragments from World
    fragments = world.fragments;
    
    entrance = {1920, 180, 40, 60};
    isUnlocked = false;
    dogFound = false;
    flickerTimer = 0.0f;
    
    std::cout << "[ZONE 4] Loaded!" << std::endl;
}

void Level1_Zone4_Hidden::Unload(World& world) {
    std::cout << "[ZONE 4] Unloading..." << std::endl;
    // Sync back to World
    world.dogPosition = dogPosition;
    skillChests.clear();
}

void Level1_Zone4_Hidden::Update(World& world, float dt) {
    if (world.player == INVALID) return;
    auto it = world.index.find(world.player);
    if (it == world.index.end()) return;
    
    size_t p = it->second;
    if (p >= world.pos.size()) return;
    
    Vector2 playerPos = world.pos[p].pos;
    
    // ✅ FIX: Sync fragments each frame
    fragments = world.fragments;
    
    // Unlock check
    if (fragments.size() >= 3 && CheckCollisionPointRec(playerPos, entrance)) {
        isUnlocked = true;
        SoundManager::Play("unlock.wav");
    }
    
    if (isUnlocked) {
        // Chest interaction
        for (auto& chest : skillChests) {
            if (!chest.opened && CheckCollisionPointRec(playerPos, chest.bounds) && IsKeyPressed(KEY_E)) {
                chest.opened = true;
                if (p < world.skill.size()) {
                    switch (chest.skillIndex) {
                        case 0: world.skill[p].z_unlock = true; break;
                        case 1: world.skill[p].x_unlock = true; break;
                        case 2: world.skill[p].c_unlock = true; break;
                        case 3: world.skill[p].v_unlock = true; break;
                    }
                }
                SoundManager::Play("chest_open.wav");
            }
        }
        
        // Dog patrol
        if (!dogFound) {
            dogPosition.x += sinf(GetTime()) * 20 * dt;
            dogPosition.x = Clamp(dogPosition.x, bounds.x + 10, bounds.x + bounds.width - 10);
        }
        
        // Dog interaction
        Rectangle dogArea = {dogPosition.x - 20, dogPosition.y - 20, 40, 40};
        if (CheckCollisionPointRec(playerPos, dogArea) && IsKeyPressed(KEY_E)) {
            dogFound = true;
            std::cout << "[ZONE 4] Found the dog!" << std::endl;
            SoundManager::Play("dog_bark.wav");
            if (p < world.skill.size()) world.skill[p].v_unlock = true;
        }
        
        // Sync back to world
        world.dogPosition = dogPosition;
    }
    
    flickerTimer += dt;
}

void Level1_Zone4_Hidden::Render() {
    // Background
    DrawRectangleGradientV(
        (int)bounds.x, (int)bounds.y,
        (int)bounds.width, (int)bounds.height,
        (Color){30, 20, 40, 255},
        (Color){10, 5, 20, 255}
    );
    
    // Entrance
    DrawRectangleRec(entrance, isUnlocked ? GREEN : RED);
    DrawText(isUnlocked ? "OPEN" : TextFormat("LOCKED (%d/3)", (int)fragments.size()),
             entrance.x + 5, entrance.y + 10, 12, WHITE);
    
    // Chests
    for (const auto& chest : skillChests) {
        DrawRectangleRec(chest.bounds, chest.opened ? GRAY : GOLD);
        DrawText(chest.opened ? "OPENED" : "E", chest.bounds.x + 10, chest.bounds.y + 10, 20, BLACK);
    }
    
    // Dog
    if (isUnlocked) {
        float flicker = (sinf(flickerTimer * 10) + 1.0f) * 0.5f;
        
        DrawCircle((int)dogPosition.x, (int)dogPosition.y, 15, ColorAlpha(BROWN, flicker));
        DrawCircle((int)dogPosition.x - 8, (int)dogPosition.y - 4, 6, ColorAlpha(DARKBROWN, flicker));
        DrawCircle((int)dogPosition.x + 8, (int)dogPosition.y - 4, 6, ColorAlpha(DARKBROWN, flicker));
        DrawCircle((int)dogPosition.x, (int)dogPosition.y + 4, 4, ColorAlpha(BLACK, flicker));
        DrawCircle((int)dogPosition.x - 4, (int)dogPosition.y - 2, 2, ColorAlpha(BLACK, flicker));
        DrawCircle((int)dogPosition.x + 4, (int)dogPosition.y - 2, 2, ColorAlpha(BLACK, flicker));
        
        if (!dogFound) {
            DrawText("Cau Vang", (int)dogPosition.x - 30, (int)dogPosition.y - 40, 16, ColorAlpha(GOLD, flicker));
            DrawText("E", (int)dogPosition.x - 5, (int)dogPosition.y + 20, 12, ColorAlpha(WHITE, flicker));
            
            // Glow effect
            float pulse = (sinf(GetTime() * 4) + 1.0f) * 0.5f;
            DrawCircleLines((int)dogPosition.x, (int)dogPosition.y, 20 + pulse * 5, ColorAlpha(YELLOW, pulse));
        }
    }
    
    // Label
    DrawText("TIEM PHO ANH HAI", (int)(bounds.x + 20), (int)(bounds.y + 20), 18, GOLD);
    
    if (isUnlocked) {
        DrawText("E to interact", (int)(bounds.x + 20), (int)(bounds.y + 200), 14, LIGHTGRAY);
    }
}