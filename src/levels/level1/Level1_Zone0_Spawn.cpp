#include "Level1_Zone0_Spawn.hpp"
#include "../../core/World.hpp"
#include "../../utills/Stubs.hpp"
#include <raymath.h>
#include <iostream>

Level1_Zone0_Spawn::Level1_Zone0_Spawn() {
    bounds = {0, 0, 5000, 5000};
}

void Level1_Zone0_Spawn::Load(World& world) {
    std::cout << "[ZONE 0] Loading Spawn Zone..." << std::endl;
    
    // Platforms - use ZonePlatform instead of PhysicsSystem::Platform
    platforms.clear();
    platforms.push_back(ZonePlatform({0, 220, 480, 20}, 1.0f));     // Main ground
    platforms.push_back(ZonePlatform({50, 200, 60, 20}, 0.8f));     // Cracked
    platforms.push_back(ZonePlatform({150, 210, 70, 10}, 1.2f));    // Sticky
    platforms.push_back(ZonePlatform({300, 190, 80, 30}, 0.6f));    // Slippery
    platforms.push_back(ZonePlatform({400, 205, 65, 15}, 0.9f));    // Normal
    
    // Tutorial triggers
    tutorialTriggers.clear();
    tutorialTriggers.push_back({{100, 150, 80, 40}, "Di chuyen: A/D hoac <-/->", false, 0.0f});
    tutorialTriggers.push_back({{250, 120, 80, 40}, "Nhay: SPACE", false, 0.0f});
    tutorialTriggers.push_back({{350, 90, 80, 40}, "Dash: SHIFT", false, 0.0f});
    
    // Create crack particles
    CreateCrackParticles();
    
    SoundManager::Play("ambient_wind.wav");
}

void Level1_Zone0_Spawn::Unload(World& world) {
    platforms.clear();
    tutorialTriggers.clear();
    crackParticles.clear();
}

void Level1_Zone0_Spawn::Update(World& world, float dt) {
    // Update crack particles
    UpdateCrackParticles(dt);
    
    // Update tutorial triggers
    if (world.player != INVALID) {
        auto playerIt = world.index.find(world.player);
        if (playerIt != world.index.end()) {
            size_t p = playerIt->second;
            
            if (p < world.pos.size() && p < world.collider.size()) {
                Rectangle playerRect = {
                    world.pos[p].pos.x - world.collider[p].rect.width/2,
                    world.pos[p].pos.y - world.collider[p].rect.height/2,
                    world.collider[p].rect.width,
                    world.collider[p].rect.height
                };
                
                for (auto& trigger : tutorialTriggers) {
                    if (!trigger.triggered && CheckCollisionRecs(playerRect, trigger.area)) {
                        trigger.triggered = true;
                        trigger.showTime = 3.0f;
                        SoundManager::Play("tutorial_popup.wav");
                    }
                    
                    if (trigger.triggered && trigger.showTime > 0) {
                        trigger.showTime -= dt;
                    }
                }
            }
        }
    }
}

void Level1_Zone0_Spawn::Render() {
    // Background - dark and ominous
    DrawRectangleGradientV(
        (int)bounds.x, (int)bounds.y,
        (int)bounds.width, (int)bounds.height,
        (Color){25, 20, 35, 255},
        (Color){15, 10, 25, 255}
    );
    
    // Main ground and platforms
    for (size_t i = 0; i < platforms.size(); i++) {
        const auto& platform = platforms[i];
        
        // Base color based on friction
        Color platformColor;
        if (platform.friction < 0.7f) {
            platformColor = (Color){100, 100, 150, 255}; // Slippery
        } else if (platform.friction > 1.3f) {
            platformColor = (Color){120, 80, 60, 255};   // Sticky
        } else {
            platformColor = (Color){70, 60, 65, 255};    // Normal
        }
        
        DrawRectangleRec(platform.bounds, platformColor);
        
        // Crack lines on main ground
        if (i == 0) {
            for (int j = 0; j < 8; j++) {
                float x = platform.bounds.x + j * 60;
                float crackLength = 10 + sinf(GetTime() + j) * 5;
                DrawLineEx(
                    {x, platform.bounds.y},
                    {x + crackLength, platform.bounds.y + platform.bounds.height},
                    1.5f, (Color){40, 35, 45, 200}
                );
            }
        }
        
        // Platform border
        DrawRectangleLinesEx(platform.bounds, 1.0f, ColorAlpha(DARKGRAY, 0.5f));
    }
    
    // Crack particles
    RenderCrackParticles();
    
    // Wind visualization
    for (int i = 0; i < 12; i++) {
        float t = fmodf(GetTime() * 1.5f + i * 0.25f, 1.0f);
        float x = windArea.x + t * windArea.width;
        float y = windArea.y + windArea.height * (i / 12.0f);
        
        // Wind particles
        DrawCircle(x, y, 2.0f, (Color){100, 180, 255, 180});
        
        // Wind direction lines
        DrawLineEx(
            {x, y},
            {x + windForce.x * 15, y + windForce.y * 15},
            1.0f, ColorAlpha(SKYBLUE, 0.7f)
        );
    }
    
    // Tutorial trigger messages
    for (const auto& trigger : tutorialTriggers) {
        if (trigger.triggered && trigger.showTime > 0) {
            float alpha = fminf(trigger.showTime, 1.0f);
            DrawRectangle(trigger.area.x - 10, trigger.area.y - 30,
                         (int)trigger.message.length() * 10 + 20, 40,
                         ColorAlpha(BLACK, alpha * 0.7f));
            
            DrawText(trigger.message.c_str(),
                    trigger.area.x, trigger.area.y - 25,
                    20, ColorAlpha(YELLOW, alpha));
        }
    }
    
    // Zone label
    DrawText("VUNG XUAT PHAT", 
            (int)(bounds.x + bounds.width/2 - 60), 
            (int)bounds.y + 20, 
            20, (Color){200, 180, 120, 255});
}

Vector2 Level1_Zone0_Spawn::GetWindForce(const Vector2& position) {
    if (CheckCollisionPointRec(position, windArea)) {
        return windForce;
    }
    return {0, 0};
}

bool Level1_Zone0_Spawn::CheckCollision(const Rectangle& rect, Vector2* correction) {
    for (const auto& platform : platforms) {
        if (CheckCollisionRecs(rect, platform.bounds)) {
            if (correction) {
                correction->y = platform.bounds.y - rect.height;
            }
            return true;
        }
    }
    return false;
}

void Level1_Zone0_Spawn::CreateCrackParticles() {
    crackParticles.clear();
    for (int i = 0; i < 20; i++) {
        Vector2 pos = {
            (float)GetRandomValue(0, 480),
            (float)GetRandomValue(200, 240)
        };
        Vector2 vel = {
            GetRandomValueFloat(-50, 50),
            GetRandomValueFloat(-100, -10)
        };
        crackParticles.push_back({pos, vel, 3.0f});
    }
}

void Level1_Zone0_Spawn::UpdateCrackParticles(float dt) {
    for (auto& p : crackParticles) {
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.velocity.y += 200.0f * dt; // Gravity
        p.lifetime -= dt;
        
        if (p.lifetime <= 0 || p.position.y > bounds.height) {
            p.lifetime = 0;
        }
    }
}

void Level1_Zone0_Spawn::RenderCrackParticles() {
    for (const auto& p : crackParticles) {
        if (p.lifetime > 0) {
            float alpha = p.lifetime / 3.0f;
            DrawCircle(p.position.x, p.position.y, 2.0f, 
                      ColorAlpha((Color){100, 100, 150, 255}, alpha));
        }
    }
}
