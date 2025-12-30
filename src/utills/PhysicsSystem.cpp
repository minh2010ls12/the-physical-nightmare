// src/systems/PhysicsSystem.cpp
#include "PhysicsSystem.hpp"
#include "../core/World.hpp"
#include "../levels/level1/Level1_ZoneBase.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

namespace PhysicsSystem {
    
    static PhysicsState* physicsState = nullptr;
    
    PhysicsState& GetPhysicsState() {
        if (!physicsState) {
            physicsState = new PhysicsState();
        }
        return *physicsState;
    }
    
    void Init() {
        std::cout << "[PHYSICS] Initializing Physics System..." << std::endl;
        PhysicsState& state = GetPhysicsState();
        state.currentZone = nullptr;
        std::cout << "[PHYSICS] System initialized (zone-based)" << std::endl;
    }
    
    void SetActiveZone(Level1_ZoneBase* zone) {
        PhysicsState& state = GetPhysicsState();
        state.currentZone = zone;
        
        if (zone) {
            std::cout << "[PHYSICS] Active zone set (platforms: " 
                      << zone->GetPlatforms().size() << ")" << std::endl;
        } else {
            std::cout << "[PHYSICS] Active zone cleared" << std::endl;
        }
    }
    
    bool CheckPlatformCollision(const Rectangle& rect, Vector2* correction) {
        PhysicsState& state = GetPhysicsState();
        
        if (!state.currentZone) {
            return false;
        }
        
        const auto& platforms = state.currentZone->GetPlatforms();
        
        for (const auto& platform : platforms) {
            if (CheckCollisionRecs(rect, platform.bounds)) {
                if (correction) {
                    float dx1 = platform.bounds.x - (rect.x + rect.width);
                    float dx2 = (platform.bounds.x + platform.bounds.width) - rect.x;
                    float dy1 = platform.bounds.y - (rect.y + rect.height);
                    float dy2 = (platform.bounds.y + platform.bounds.height) - rect.y;
                    
                    float minPenetration = fminf(fminf(fabsf(dx1), fabsf(dx2)), 
                                                fminf(fabsf(dy1), fabsf(dy2)));
                    
                    if (fabsf(dx1) == minPenetration) {
                        correction->x = dx1;
                        correction->y = 0;
                    } else if (fabsf(dx2) == minPenetration) {
                        correction->x = dx2;
                        correction->y = 0;
                    } else if (fabsf(dy1) == minPenetration) {
                        correction->x = 0;
                        correction->y = dy1;
                    } else {
                        correction->x = 0;
                        correction->y = dy2;
                    }
                }
                return true;
            }
        }
        return false;
    }
    
    Vector2 GetWindForce(const Vector2& position) {
        PhysicsState& state = GetPhysicsState();
        
        if (state.currentZone) {
            return state.currentZone->GetWindForce(position);
        }
        
        return {state.globalWindX, state.globalWindY};
    }
    
    float ApplyFriction(float velocity, float friction, bool grounded, float dt) {
        if (grounded && fabsf(velocity) > 0.1f) {
            velocity *= powf(friction, dt * 60.0f);
            if (fabsf(velocity) < 0.1f) velocity = 0.0f;
        } else {
            velocity *= powf(AIR_RESISTANCE, dt * 60.0f);
        }
        return velocity;
    }
    
    void UpdatePlayerPhysics(World& world, size_t playerIdx, float dt) {
        if (playerIdx >= world.pos.size() || playerIdx >= world.velocity.size() || 
            playerIdx >= world.input.size() || playerIdx >= world.collider.size()) {
            return;
        }
        
        auto& pos = world.pos[playerIdx];
        auto& vel = world.velocity[playerIdx];
        auto& input = world.input[playerIdx];
        auto& collider = world.collider[playerIdx];
        
        Rectangle playerRect = {
            pos.pos.x - collider.rect.width/2 + collider.offset.x,
            pos.pos.y - collider.rect.height/2 + collider.offset.y,
            collider.rect.width,
            collider.rect.height
        };
        
        Rectangle feetSensor = {
            playerRect.x + playerRect.width * 0.2f,
            playerRect.y + playerRect.height - 2.0f,
            playerRect.width * 0.6f,
            4.0f
        };
        
        Vector2 groundCorrection;
        input.grounded = CheckPlatformCollision(feetSensor, &groundCorrection);
        
        constexpr float COYOTE_TIME = 0.12f;
        if (input.grounded) {
            input.coyote = COYOTE_TIME;
            input.can_double = true;
        } else {
            input.coyote -= dt;
        }
        
        bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        bool jumpPressed = IsKeyPressed(KEY_SPACE);
        bool dashPressed = IsKeyPressed(KEY_LEFT_SHIFT);
        
        constexpr float DASH_SPEED = 800.0f;
        constexpr float DASH_DURATION = 0.2f;
        
        if (dashPressed && input.dash_time <= 0) {
            input.dashing = true;
            input.dash_time = DASH_DURATION;
            vel.vel.x = pos.facing * DASH_SPEED;
            vel.vel.y = 0;
        }
        
        if (input.dash_time > 0) {
            input.dash_time -= dt;
            if (input.dash_time <= 0) {
                input.dashing = false;
            }
        }
        
        if (!input.dashing) {
            float targetSpeed = 0.0f;
            if (right && !left) targetSpeed = 250.0f;
            else if (left && !right) targetSpeed = -250.0f;
            
            float acceleration = input.grounded ? 1800.0f : 600.0f;
            if (targetSpeed > vel.vel.x) {
                vel.vel.x = fminf(vel.vel.x + acceleration * dt, targetSpeed);
            } else if (targetSpeed < vel.vel.x) {
                vel.vel.x = fmaxf(vel.vel.x - acceleration * dt, targetSpeed);
            }
            
            if (right && !left) pos.facing = 1.0f;
            else if (left && !right) pos.facing = -1.0f;
            
            vel.vel.x = ApplyFriction(vel.vel.x, FRICTION, input.grounded, dt);
        }
        
        bool canJump = (input.grounded || input.coyote > 0);
        
        if (jumpPressed && canJump) {
            vel.vel.y = -420.0f;
            input.coyote = 0;
        }
        
        if (jumpPressed && !input.grounded && input.can_double) {
            vel.vel.y = -420.0f;
            input.can_double = false;
        }
        
        if (!input.grounded) {
            vel.vel.y += GRAVITY * dt;
            vel.vel.y = fminf(vel.vel.y, MAX_FALL_SPEED);
        } else if (vel.vel.y > 0) {
            vel.vel.y = 0;
        }
        
        Vector2 windForce = GetWindForce(pos.pos);
        vel.vel.x += windForce.x * dt;
        vel.vel.y += windForce.y * dt;
        
        Vector2 movement = {vel.vel.x * dt, vel.vel.y * dt};
        
        if (movement.x != 0) {
            pos.pos.x += movement.x;
            playerRect.x += movement.x;
            
            Vector2 correction;
            if (CheckPlatformCollision(playerRect, &correction)) {
                pos.pos.x += correction.x;
                vel.vel.x = 0;
            }
        }
        
        if (movement.y != 0) {
            pos.pos.y += movement.y;
            playerRect.y += movement.y;
            
            Vector2 correction;
            if (CheckPlatformCollision(playerRect, &correction)) {
                pos.pos.y += correction.y;
                vel.vel.y = 0;
            }
        }
    }
    
    void Update(World& world, float dt) {
        if (world.player != INVALID) {
            auto it = world.index.find(world.player);
            if (it != world.index.end()) {
                UpdatePlayerPhysics(world, it->second, dt);
            }
        }
    }
    
    void RenderDebug() {
        if (!IsKeyDown(KEY_F3)) return;
        
        PhysicsState& state = GetPhysicsState();
        
        if (!state.currentZone) {
            DrawText("NO ACTIVE ZONE", 10, 50, 20, RED);
            return;
        }
        
        const auto& platforms = state.currentZone->GetPlatforms();
        
        for (const auto& platform : platforms) {
            Color platformColor = (platform.friction < 1.0f) ? ColorAlpha(RED, 0.3f) : 
                                (platform.friction > 1.0f) ? ColorAlpha(GREEN, 0.3f) : 
                                ColorAlpha(GRAY, 0.3f);
            
            DrawRectangleRec(platform.bounds, platformColor);
            DrawRectangleLinesEx(platform.bounds, 1.0f, platformColor);
            
            DrawText(TextFormat("f:%.1f", platform.friction),
                    platform.bounds.x + 5, platform.bounds.y - 15,
                    10, WHITE);
        }
        
        DrawText("PHYSICS DEBUG (F3)", 10, 50, 20, YELLOW);
        DrawText(TextFormat("Platforms: %d", (int)platforms.size()), 10, 80, 15, WHITE);
    }
    
    void Cleanup() {
        if (physicsState) {
            delete physicsState;
            physicsState = nullptr;
        }
    }
    
} // namespace PhysicsSystem