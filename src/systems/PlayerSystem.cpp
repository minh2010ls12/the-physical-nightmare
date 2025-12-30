// src/systems/PlayerSystem.cpp
#include "PlayerSystem.hpp"
#include "../utills/PhysicsSystem.hpp"
#include "../skills/Skill.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

void PlayerSystem_Update(World& world, float dt) {
    if (world.player == INVALID) {
        std::cout << "[PLAYER] Player is INVALID, skipping update" << std::endl;
        return;
    }
    
    auto it = world.index.find(world.player);
    if (it == world.index.end()) {
        std::cout << "[PLAYER] Player not found in index, skipping update" << std::endl;
        return;
    }
    
    size_t p = it->second;
    
    // ✅ CRITICAL: Validate ALL indices before ANY access
    if (p >= world.pos.size()) {
        std::cout << "[PLAYER] ERROR: Player index " << p << " >= pos.size() " << world.pos.size() << std::endl;
        return;
    }
    if (p >= world.velocity.size()) {
        std::cout << "[PLAYER] ERROR: Player index " << p << " >= velocity.size() " << world.velocity.size() << std::endl;
        return;
    }
    if (p >= world.input.size()) {
        std::cout << "[PLAYER] ERROR: Player index " << p << " >= input.size() " << world.input.size() << std::endl;
        return;
    }
    if (p >= world.collider.size()) {
        std::cout << "[PLAYER] ERROR: Player index " << p << " >= collider.size() " << world.collider.size() << std::endl;
        return;
    }
    if (p >= world.health.size()) {
        std::cout << "[PLAYER] ERROR: Player index " << p << " >= health.size() " << world.health.size() << std::endl;
        return;
    }
    
    // Now safe to access
    Vector2& pos = world.pos[p].pos;
    Vector2& vel = world.velocity[p].vel;
    PlayerInput& input = world.input[p];
    float& facing = world.pos[p].facing;
    
    // Movement input
    float moveX = 0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) moveX = -1;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) moveX = 1;
    
    // Constants
    const float MOVE_SPEED = 300.0f;
    const float JUMP_FORCE = -500.0f;
    const float DASH_SPEED = 800.0f;
    const float DASH_DURATION = 0.2f;
    const int DASH_MANA_COST = 10;
    
    // Update facing direction
    if (moveX != 0) facing = moveX;
    
    // Dash logic
    if (!input.dashing) {
        if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
            if (p < world.infected.size() && world.infected[p].phsp >= DASH_MANA_COST) {
                input.dashing = true;
                input.dash_time = DASH_DURATION;
                
                world.infected[p].phsp -= DASH_MANA_COST;
                
                vel.x = facing * DASH_SPEED;
                vel.y = 0;
                
                world.health[p].invincible = true;
            }
        }
    }
    
    // Handle dashing state
    if (input.dashing) {
        input.dash_time -= dt;
        
        if (input.dash_time <= 0) {
            input.dashing = false;
            world.health[p].invincible = false;
        }
    }
    
    // Normal movement (not dashing)
    if (!input.dashing) {
        vel.x = moveX * MOVE_SPEED;
        
        // Gravity
        vel.y += 800.0f * dt;
        
        // Ground check
        input.grounded = PhysicsSystem::CheckGround(pos, {16, 24});
        
        // Coyote time
        if (input.grounded) {
            input.coyote = 0.15f;
            input.can_double = true;
        } else {
            input.coyote -= dt;
        }
        
        // Jump
        if (IsKeyPressed(KEY_SPACE)) {
            if (input.coyote > 0) {
                vel.y = JUMP_FORCE;
                input.coyote = 0;
            } else if (input.can_double) {
                vel.y = JUMP_FORCE;
                input.can_double = false;
            }
        }
        
        // Variable jump height
        if (IsKeyReleased(KEY_SPACE) && vel.y < 0) {
            vel.y *= 0.5f;
        }
    }
    
    // Apply velocity
    pos.x += vel.x * dt;
    pos.y += vel.y * dt;
    
    // Ground collision
    if (input.grounded && pos.y > 4700) {
        pos.y = 4700;
        vel.y = 0;
    }
    
    // World bounds
    pos.x = Clamp(pos.x, 0.0f, 10000.0f);
    pos.y = Clamp(pos.y, -1000.0f, 5000.0f);
    
    // Skill inputs - with bounds check
    if (p < world.skill.size()) {
        // Z - Plasma
        if (IsKeyPressed(KEY_Z) && world.skill[p].z_unlock) {
            if (world.playerSkills[0] != nullptr) {
                world.playerSkills[0]->Activate(world, p);
            }
        }
        
        // X - Shield
        if (IsKeyPressed(KEY_X) && world.skill[p].x_unlock) {
            if (world.playerSkills[1] != nullptr) {
                world.playerSkills[1]->Activate(world, p);
            }
        }
        
        // C - Bomb
        if (IsKeyPressed(KEY_C) && world.skill[p].c_unlock) {
            if (world.playerSkills[2] != nullptr) {
                world.playerSkills[2]->Activate(world, p);
            }
        }
        
        // V - Ultimate
        if (IsKeyPressed(KEY_V) && world.skill[p].v_unlock) {
            if (world.playerSkills[3] != nullptr) {
                world.playerSkills[3]->Activate(world, p);
            }
        }
        
        // Update all skills
        for (int i = 0; i < 4; i++) {
            if (world.playerSkills[i] != nullptr) {
                world.playerSkills[i]->Update(world, p, dt);
            }
        }
    }
}