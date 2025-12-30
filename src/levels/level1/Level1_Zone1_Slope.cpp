// src/levels/level1/Level1_Zone1_Slope.cpp
#include "Level1_Zone1_Slope.hpp"
#include "../../utills/Stubs.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

// ========== PUBLIC METHODS ==========

void Level1_Zone1_Slope::Load(World& world) {
    std::cout << "[ZONE 1] Loading Slope Zone..." << std::endl;
    
    bounds = {0, 0, 5000, 5000};
    
    // Clear existing data
    slopeSegments.clear();
    inertiaEnemies.clear();
    platforms.clear();
    
    // Create slope segments
    CreateSlopeSegments();
    
    // Create platforms - DÙNG ZonePlatform
    platforms.push_back(ZonePlatform({500, 180, 100, 20}, 0.3f));
    platforms.push_back(ZonePlatform({650, 150, 120, 20}, 1.5f));
    platforms.push_back(ZonePlatform({800, 200, 80, 20}, 0.8f));
    
    // Create inertia enemies
    CreateInertiaEnemies(world);
    
    SoundManager::Play("wind_slope.wav");
}

void Level1_Zone1_Slope::Unload(World& world) {
    std::cout << "[ZONE 1] Unloading Slope Zone..." << std::endl;
    
    // Destroy enemies
    for (auto enemy : inertiaEnemies) {
        world.Destroy(enemy);
    }
    
    slopeSegments.clear();
    inertiaEnemies.clear();
    platforms.clear();
}

void Level1_Zone1_Slope::Update(World& world, float dt) {
    // Update inertia enemies
    for (size_t i = 0; i < inertiaEnemies.size(); i++) {
        Entity enemy = inertiaEnemies[i];
        auto it = world.index.find(enemy);
        if (it != world.index.end()) {
            size_t idx = it->second;
            
            if (idx < world.pos.size() && idx < world.velocity.size()) {
                Vector2& pos = world.pos[idx].pos;
                Vector2& vel = world.velocity[idx].vel;
                
                // Simple AI
                if (idx < world.ai.size()) {
                    world.ai[idx].timer += dt;
                    if (world.ai[idx].timer > 2.0f) {
                        world.ai[idx].timer = 0;
                        world.ai[idx].type = (world.ai[idx].type == 1) ? 2 : 1;
                    }
                    
                    if (world.ai[idx].type == 1) {
                        vel.x = 80.0f;
                    } else {
                        vel.x = -80.0f;
                    }
                }
                
                // Apply gravity
                vel.y += 800.0f * dt;
                
                // Update position
                pos.x += vel.x * dt;
                pos.y += vel.y * dt;
                
                // Check ground collision
                float groundY = GetYOnSlope(pos.x);
                if (pos.y > groundY - 10) {
                    pos.y = groundY - 10;
                    vel.y = 0;
                }
                
                // Boundary check
                if (pos.x < bounds.x + 30) {
                    pos.x = bounds.x + 30;
                    vel.x = 80.0f;
                }
                if (pos.x > bounds.x + bounds.width - 30) {
                    pos.x = bounds.x + bounds.width - 30;
                    vel.x = -80.0f;
                }
            }
        }
    }
}

void Level1_Zone1_Slope::Render() {
    // Background
    DrawRectangleGradientV(
        (int)bounds.x, (int)bounds.y,
        (int)bounds.width, (int)bounds.height,
        (Color){40, 35, 50, 255},
        (Color){30, 25, 40, 255}
    );
    
    // Draw slope segments
    for (const auto& segment : slopeSegments) {
        // Draw slope line
        DrawLineEx({segment.x1, segment.y1}, {segment.x2, segment.y2}, 
                   5.0f, (Color){120, 100, 80, 255});
        
        // Draw slope fill
        for (float x = segment.x1; x < segment.x2; x += 5.0f) {
            float t = (x - segment.x1) / (segment.x2 - segment.x1);
            float y1 = segment.y1 + t * (segment.y2 - segment.y1);
            float y2 = bounds.y + bounds.height;
            
            Color segmentColor;
            if (segment.friction < 0.5f) {
                segmentColor = (Color){100, 150, 200, 150};
            } else if (segment.friction > 1.5f) {
                segmentColor = (Color){120, 80, 60, 150};
            } else {
                segmentColor = (Color){100, 80, 60, 150};
            }
            
            DrawLine((int)x, (int)y1, (int)x, (int)y2, segmentColor);
        }
    }
    
    // Draw platforms
    for (const auto& platform : platforms) {
        Color platformColor;
        if (platform.friction < 0.5f) {
            platformColor = SKYBLUE;
        } else if (platform.friction > 1.5f) {
            platformColor = BROWN;
        } else {
            platformColor = (Color){150, 120, 100, 255};
        }
        
        DrawRectangleRec(platform.bounds, ColorAlpha(platformColor, 0.8f));
        DrawRectangleLinesEx(platform.bounds, 2.0f, ColorAlpha(platformColor, 1.0f));
    }
    
    // Draw zone label
    DrawText("THUNG LUNG DOI DOC",
            (int)(bounds.x + bounds.width/2 - 100),
            (int)bounds.y + 20,
            22, (Color){200, 180, 120, 255});
}

// ========== PRIVATE HELPER METHODS ==========

void Level1_Zone1_Slope::CreateSlopeSegments() {
    slopeSegments.clear();
    
    slopeSegments.push_back({500, 200, 600, 180, 15.0f * DEG2RAD, 0.7f});
    slopeSegments.push_back({600, 180, 700, 140, 30.0f * DEG2RAD, 1.0f});
    slopeSegments.push_back({700, 140, 800, 100, 45.0f * DEG2RAD, 1.8f});
    slopeSegments.push_back({800, 100, 900, 160, -25.0f * DEG2RAD, 0.4f});
}

void Level1_Zone1_Slope::CreateInertiaEnemies(World& world) {
    for (auto enemy : inertiaEnemies) {
        world.Destroy(enemy);
    }
    inertiaEnemies.clear();
    
    for (int i = 0; i < 3; i++) {
        Entity enemy = world.Create();
        size_t idx = world.index[enemy];
        
        float xPos = bounds.x + 100 + i * 150;
        float yPos = GetYOnSlope(xPos) - 40;
        
        if (idx < world.pos.size()) {
            world.pos[idx].pos = {xPos, yPos};
        }
        
        if (idx < world.ai.size()) {
            world.ai[idx].type = 1 + (i % 2);
            world.ai[idx].timer = 0;
        }
        
        if (idx < world.health.size()) {
            world.health[idx].hp = 50;
        }
        
        if (idx < world.sprite.size()) {
            if (i == 0) world.sprite[idx].color = BLUE;
            else if (i == 1) world.sprite[idx].color = BROWN;
            else world.sprite[idx].color = GRAY;
        }
        
        inertiaEnemies.push_back(enemy);
    }
}

float Level1_Zone1_Slope::GetYOnSlope(float x) const {
    for (const auto& segment : slopeSegments) {
        if (x >= segment.x1 && x <= segment.x2) {
            float t = (x - segment.x1) / (segment.x2 - segment.x1);
            return segment.y1 + t * (segment.y2 - segment.y1);
        }
    }
    
    return bounds.y + bounds.height - 20;
}

float Level1_Zone1_Slope::GetFriction(float x, float y) const {
    for (const auto& segment : slopeSegments) {
        if (x >= segment.x1 - 10 && x <= segment.x2 + 10) {
            float slopeY = GetYOnSlope(x);
            if (fabsf(y - slopeY) < 20) {
                return segment.friction;
            }
        }
    }
    
    for (const auto& platform : platforms) {
        if (CheckCollisionPointRec({x, y}, platform.bounds)) {
            return platform.friction;
        }
    }
    
    return 1.0f;
}

bool Level1_Zone1_Slope::CheckSlopeCollision(const Rectangle& rect, Vector2* correction) const {
    float minY = GetYOnSlope(rect.x);
    float maxY = GetYOnSlope(rect.x + rect.width);
    float groundY = fminf(minY, maxY);
    
    if (rect.y + rect.height > groundY) {
        if (correction) {
            correction->x = 0;
            correction->y = groundY - (rect.y + rect.height);
        }
        return true;
    }
    
    return false;
}