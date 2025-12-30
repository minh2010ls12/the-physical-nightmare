// src/levels/level1/Level1_Zone6_Exit.hpp
#pragma once
#include "Level1_ZoneBase.hpp"
class Level1_Zone6_Exit : public Level1_ZoneBase {
private:
    bool portalActive = false;
public:
    Level1_Zone6_Exit() {
        bounds = {7500, 0, 1500, 5000};
    }
    void Load(World& world) override {
        platforms.clear();
        platforms.push_back(ZonePlatform({7500, 1800, 300, 30}, 1.0f));
        platforms.push_back(ZonePlatform({7650, 1600, 250, 25}, 1.0f));
        platforms.push_back(ZonePlatform({7800, 1400, 250, 25}, 1.0f));
        platforms.push_back(ZonePlatform({7950, 1200, 250, 25}, 1.0f));
        platforms.push_back(ZonePlatform({8100, 1000, 400, 40}, 1.0f)); 
        portalActive = false;
    }
    
    void Unload(World& world) override {
        platforms.clear();
        enemies.clear();
    }
    
    void Update(World& world, float dt) override {
        // Check if boss is defeated (simplified - check if Zone 5 enemies are dead)
        portalActive = true; // For now, always active
        
        // Check if player reached exit
        if (world.player != INVALID && portalActive) {
            auto playerIt = world.index.find(world.player);
            if (playerIt != world.index.end()) {
                size_t p = playerIt->second;
                if (p < world.pos.size()) {
                    Vector2 playerPos = world.pos[p].pos;
                    
                    Rectangle exitZone = {8100, 800, 400, 200};
                    if (CheckCollisionPointRec(playerPos, exitZone)) {
                        std::cout << "[ZONE 6] Player reached exit!" << std::endl;
                        // TODO: Trigger level complete
                    }
                }
            }
        }
    }
    
    void Render() override {
        // Peaceful exit zone background
        DrawRectangleGradientV(
            bounds.x, bounds.y,
            bounds.width, bounds.height,
            (Color){30, 40, 60, 255},
            (Color){15, 20, 35, 255}
        );
        
        // Render platforms
        for (const auto& platform : platforms) {
            DrawRectangleRec(platform.bounds, (Color){60, 70, 90, 255});
            DrawRectangleLinesEx(platform.bounds, 2.0f, (Color){90, 100, 130, 255});
        }
        
        // Portal effect at exit
        if (portalActive) {
            float portalX = 8300;
            float portalY = 900;
            
            // Rotating portal rings
            for (int i = 0; i < 5; i++) {
                float radius = 50 + i * 20;
                float rotation = GetTime() * (i % 2 == 0 ? 2.0f : -2.0f);
                float alpha = 1.0f - (i * 0.15f);
                
                DrawCircleLines(portalX, portalY, radius, 
                              ColorAlpha(SKYBLUE, alpha));
            }
            
            // Portal center glow
            float pulse = sinf(GetTime() * 3) * 0.3f + 0.7f;
            DrawCircle(portalX, portalY, 30, ColorAlpha(WHITE, pulse));
            DrawCircle(portalX, portalY, 50, ColorAlpha(SKYBLUE, pulse * 0.5f));
            
            // Exit text
            DrawText("LỖNG THOÁT",
                    portalX - 80, portalY - 150,
                    30, ColorAlpha(SKYBLUE, pulse));
        }
        
        // Zone label
        DrawText("LỐI THOÁT",
                bounds.x + bounds.width/2 - 80,
                bounds.y + 50,
                30, (Color){200, 220, 255, 255});
    }
};