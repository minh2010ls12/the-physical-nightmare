#include "Level1_Zone5_Boss.hpp"
#include "../../utills/Stubs.hpp"
#include <iostream>

void Level1_Zone5_Boss::Load(World& world) {
    std::cout << "[ZONE 5] Loading Boss Zone..." << std::endl;
    
    bounds = {0, 0, 2000, 2000};
    pillars.clear();
    
    pillars.push_back({2250, 170, 20, 70});
    pillars.push_back({2400, 160, 20, 80});
    pillars.push_back({2550, 150, 20, 90});
    
    boss = world.Create();
    if (world.index.count(boss)) {
        size_t idx = world.index[boss];
        
        if (idx < world.pos.size()) {
            world.pos[idx].pos = {2400, 80};
        }
        if (idx < world.health.size()) {
            world.health[idx].hp = 5000;
            world.health[idx].max_hp = 5000;
        }
        if (idx < world.sprite.size()) {
            world.sprite[idx].color = RED;
        }
        if (idx < world.ai.size()) {
            world.ai[idx].type = 888;
            world.ai[idx].timer = 0;
        }
    }
    
    SoundManager::Play("boss_intro.wav");
}

void Level1_Zone5_Boss::Unload(World& world) {
    std::cout << "[ZONE 5] Unloading Boss Zone..." << std::endl;
    
    if (boss != INVALID) {
        world.Destroy(boss);
        boss = INVALID;
    }
    pillars.clear();
}

void Level1_Zone5_Boss::Update(World& world, float dt) {
    if (boss == INVALID || !world.index.count(boss)) return;
    
    size_t bIdx = world.index[boss];
    if (bIdx >= world.pos.size()) return;
    
    if (bIdx < world.ai.size()) {
        world.ai[bIdx].timer += dt;
        
        if (world.ai[bIdx].timer > 3.0f) {
            world.ai[bIdx].timer = 0;
            
            if (bIdx < world.velocity.size()) {
                world.velocity[bIdx].vel.x = (GetRandomValue(0, 1) ? 200 : -200);
            }
        }
    }
    
    if (bIdx < world.velocity.size()) {
        world.velocity[bIdx].vel.y += 800.0f * dt;
        world.pos[bIdx].pos.y += world.velocity[bIdx].vel.y * dt;
        world.pos[bIdx].pos.x += world.velocity[bIdx].vel.x * dt;
        
        if (world.pos[bIdx].pos.y > bounds.y + bounds.height - 30) {
            world.pos[bIdx].pos.y = bounds.y + bounds.height - 30;
            world.velocity[bIdx].vel.y = 0;
        }
    }
}

void Level1_Zone5_Boss::Render() {
    // Dark boss arena
    DrawRectangleGradientV(
        (int)bounds.x, (int)bounds.y,
        (int)bounds.width, (int)bounds.height,
        (Color){80, 30, 30, 255},
        (Color){50, 10, 10, 255}
    );
    
    // Draw pillars
    for (const auto& pillar : pillars) {
        DrawRectangleRec(pillar, (Color){150, 100, 80, 255});
        DrawRectangleLinesEx(pillar, 2.0f, BROWN);
    }
    
    // Boss visual (simple representation)
    DrawRectangle((int)(bounds.x + bounds.width/2) - 40, 80, 80, 60, RED);
    DrawRectangleLinesEx({(float)((int)(bounds.x + bounds.width/2) - 40), 80.0f, 80, 60}, 3.0f, Color{90, 0, 0, 255});
    
    // Eyes
    DrawCircle((int)(bounds.x + bounds.width/2) - 15, 95, 5, YELLOW);
    DrawCircle((int)(bounds.x + bounds.width/2) + 15, 95, 5, YELLOW);
    
    // Health bar
    DrawRectangle((int)(bounds.x + 50), 10, 300, 15, BLACK);
    DrawRectangle((int)(bounds.x + 50), 10, 300, 15, RED);
    
    DrawText("BAO CHU A F=ma",
            (int)(bounds.x + bounds.width/2 - 100),
            (int)bounds.y + 20,
            22, (Color){255, 100, 100, 255});
}