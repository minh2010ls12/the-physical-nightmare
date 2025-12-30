// src/levels/level1/Level1_Zone2_Cave.hpp
#pragma once
#include "Level1_ZoneBase.hpp"
#include <vector>

class Level1_Zone2_Cave : public Level1_ZoneBase {
public:
    Level1_Zone2_Cave() {
        bounds = {2000, 0, 2000, 5000};
    }
    
    void Load(World& world) override {
        (void)world;
        
        platforms.clear();
        
        // Cave platforms - narrow passages
        platforms.push_back(ZonePlatform({2000, 4700, 300, 30}, 1.0f));
        platforms.push_back(ZonePlatform({2200, 4500, 200, 20}, 1.0f));
        platforms.push_back(ZonePlatform({2050, 4300, 250, 20}, 1.0f));
        platforms.push_back(ZonePlatform({2300, 4100, 300, 30}, 1.0f));
        platforms.push_back(ZonePlatform({2100, 3900, 400, 40}, 1.0f));
        
        // Ceiling platforms (stalactites)
        platforms.push_back(ZonePlatform({2150, 4600, 100, 50}, 1.0f));
        platforms.push_back(ZonePlatform({2400, 4450, 80, 40}, 1.0f));
        
        // ✅ Initialize gas vents
        vents.clear();
        vents.push_back({{2100, 4600}, 0.0f});
        vents.push_back({{2300, 4400}, 0.5f});
        vents.push_back({{2150, 4200}, 1.0f});
    }
    
    void Unload(World& world) override {
        // Clean enemies
        for (auto it = enemies.begin(); it != enemies.end();) {
            if (world.IsEntityValid(*it)) {
                world.Destroy(*it);
            }
            it = enemies.erase(it);
        }
        
        platforms.clear();
        vents.clear();
    }
    
    void Update(World& world, float dt) override {
        (void)world;
        
        // Update gas vents
        for (auto& vent : vents) {
            vent.timer += dt;
            if (vent.timer >= 2.0f) {
                vent.timer = 0.0f;
                // TODO: Spawn gas particles
            }
        }
    }
    
    void Render() override {
        // Very dark cave background
        DrawRectangle(
            bounds.x, bounds.y,
            bounds.width, bounds.height,
            (Color){5, 3, 10, 255}
        );
        
        // Render platforms as rocky surfaces
        for (const auto& platform : platforms) {
            DrawRectangleRec(platform.bounds, (Color){50, 40, 55, 255});
            
            // Rock texture lines
            for (int i = 0; i < 3; i++) {
                float x = platform.bounds.x + GetRandomValue(0, (int)platform.bounds.width);
                DrawLineEx(
                    {x, platform.bounds.y},
                    {x + GetRandomValue(-10, 10), platform.bounds.y + platform.bounds.height},
                    1.0f, (Color){30, 25, 35, 255}
                );
            }
            
            DrawRectangleLinesEx(platform.bounds, 1.5f, (Color){70, 60, 80, 255});
        }
        
        // Render gas vents
        for (const auto& vent : vents) {
            float pulse = sinf(GetTime() * 3 + vent.pos.x) * 0.3f + 0.7f;
            DrawCircle(vent.pos.x, vent.pos.y, 15, ColorAlpha(GREEN, pulse * 0.6f));
            DrawCircleLines(vent.pos.x, vent.pos.y, 20, ColorAlpha(LIME, pulse));
        }
        
        // Glowing crystals for atmosphere
        for (int i = 0; i < 8; i++) {
            float x = bounds.x + (i * bounds.width / 8);
            float y = bounds.y + GetRandomValue(100, 1000);
            float pulse = sinf(GetTime() * 3 + i) * 0.3f + 0.7f;
            
            DrawCircle(x, y, 5, ColorAlpha((Color){100, 200, 255, 255}, pulse));
        }
        
        // Zone label
        DrawText("HANG ĐỘNG TỐI",
                bounds.x + bounds.width/2 - 100,
                bounds.y + 50,
                30, ColorAlpha(SKYBLUE, 0.8f));
    }
    
private:
    std::vector<GasVent> vents;
};