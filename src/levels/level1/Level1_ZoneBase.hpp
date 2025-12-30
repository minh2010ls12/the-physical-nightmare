// src/levels/level1/Level1_ZoneBase.hpp
#pragma once
#include "../../core/World.hpp"
#include "../ZonePlatform.hpp"
#include <raylib.h>
#include <vector>
#include <string>

// ✅ FIX: Add missing structs
struct TutorialTrigger {
    Rectangle area;
    std::string message;
    bool triggered;
    float showTime;
};

struct GasVent {
    Vector2 pos;
    float timer;
};

// Base class for all zones
class Level1_ZoneBase {
public:
    virtual ~Level1_ZoneBase() = default;
    
    // Core methods
    virtual void Load(World& world) = 0;
    virtual void Unload(World& world) = 0;
    virtual void Update(World& world, float dt) = 0;
    virtual void Render() = 0;
    
    // Physics & collision (suppress warnings with void cast)
    virtual Vector2 GetWindForce(const Vector2& position) { 
        (void)position; 
        return {0, 0}; 
    }
    
    virtual bool CheckCollision(const Rectangle& rect, Vector2* correction = nullptr) { 
        (void)rect; 
        (void)correction; 
        return false; 
    }
    
    // Zone properties
    Rectangle GetBounds() const { return bounds; }
    bool IsInZone(const Vector2& pos) const { 
        return CheckCollisionPointRec(pos, bounds); 
    }
    
    // Allow PhysicsSystem to access platforms
    const std::vector<ZonePlatform>& GetPlatforms() const { return platforms; }
    
protected:
    Rectangle bounds = {0, 0, 5000, 5000};
    std::vector<ZonePlatform> platforms;
    std::vector<Entity> enemies;
};