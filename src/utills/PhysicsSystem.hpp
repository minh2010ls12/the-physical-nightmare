// src/systems/PhysicsSystem.hpp
#pragma once
#include "../core/World.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>

// Forward declaration
class Level1_ZoneBase;

namespace PhysicsSystem {

    constexpr float GRAVITY = 1400.0f;
    constexpr float MAX_FALL_SPEED = 1200.0f;
    constexpr float FRICTION = 0.85f;
    constexpr float AIR_RESISTANCE = 0.95f;
    
    // ✅ SIMPLIFIED: Zone-based physics state
    struct PhysicsState {
        Level1_ZoneBase* currentZone = nullptr;  // Active zone for physics
        float globalWindX = 0.0f;
        float globalWindY = 0.0f;
    };
    
    PhysicsState& GetPhysicsState();
    
    void Init();
    
    // ✅ NEW: Set active zone for physics
    void SetActiveZone(Level1_ZoneBase* zone);
    
    // ✅ SIMPLIFIED: Only one CheckPlatformCollision signature
    bool CheckPlatformCollision(const Rectangle& rect, Vector2* correction = nullptr);
    
    // Ground check helper
    inline bool CheckGround(const Vector2& pos, const Vector2& size) {
        Rectangle feetSensor = {
            pos.x - size.x/2,
            pos.y + size.y/2 - 4,
            size.x,
            4
        };
        return CheckPlatformCollision(feetSensor);
    }
    
    Vector2 GetWindForce(const Vector2& position);
    float ApplyFriction(float velocity, float friction, bool grounded, float dt);
    
    void UpdatePlayerPhysics(World& world, size_t playerIdx, float dt);
    void Update(World& world, float dt);
    void RenderDebug();
    void Cleanup();
    
} // namespace PhysicsSystem