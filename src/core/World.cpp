// src/core/World.cpp
#include "World.hpp"
#include "../skills/Skill.hpp"
#include <algorithm>
#include <iostream>

Entity World::Create() {
    Entity e = next_id++;
    
    pos.push_back(Pos{});
    velocity.push_back(Velocity{});
    collider.push_back(Collider{});
    input.push_back(PlayerInput{});
    sprite.push_back(Sprite{});
    health.push_back(Health{});
    ai.push_back(EnemyAI{});
    skill.push_back(SkillState{});
    infected.push_back(Infected{});
    
    size_t idx = pos.size() - 1;
    index[e] = idx;
    
    std::cout << "[WORLD] Created entity " << e << " at index " << idx << std::endl;
    return e;
}

void World::Destroy(Entity e) {
    auto it = index.find(e);
    if (it == index.end()) {
        std::cout << "[WORLD] Warning: Trying to destroy non-existent entity " << e << std::endl;
        return;
    }
    
    size_t idx = it->second;
    std::cout << "[WORLD] Destroying entity " << e << " at index " << idx << std::endl;
    
    if (idx < ai.size()) {
        ai[idx].type = -1;
    }
    
    index.erase(it);
    
    if (e == player) {
        player = INVALID;
        std::cout << "[WORLD] Player entity destroyed" << std::endl;
    }
}

void World::CleanupDestroyedEntities() {
    std::vector<Entity> toRemove;
    
    // ✅ FIX: Find destroyed entities
    for (const auto& pair : index) {
        Entity e = pair.first;
        size_t idx = pair.second;
        
        if (idx < ai.size() && ai[idx].type == -1) {
            toRemove.push_back(e);
        }
    }
    
    if (toRemove.empty()) {
        return;
    }
    
    std::cout << "[WORLD] Cleaning up " << toRemove.size() << " destroyed entities" << std::endl;
    
    for (Entity e : toRemove) {
        auto it = index.find(e);
        if (it == index.end()) continue;
        
        size_t idx = it->second;
        size_t last = pos.size() - 1;
        
        std::cout << "[WORLD] Swap-remove entity " << e << " (idx=" << idx << ", last=" << last << ")" << std::endl;
        
        // Remove from index first
        index.erase(it);
        
        if (idx != last) {
            // Swap with last
            pos[idx] = std::move(pos[last]);
            velocity[idx] = std::move(velocity[last]);
            collider[idx] = std::move(collider[last]);
            input[idx] = std::move(input[last]);
            sprite[idx] = std::move(sprite[last]);
            health[idx] = std::move(health[last]);
            ai[idx] = std::move(ai[last]);
            skill[idx] = std::move(skill[last]);
            infected[idx] = std::move(infected[last]);
            
            // ✅ CRITICAL FIX: Find entity at 'last' and update its index
            Entity entityAtLast = INVALID;
            for (const auto& p : index) {
                if (p.second == last) {
                    entityAtLast = p.first;
                    break;
                }
            }
            
            if (entityAtLast != INVALID) {
                std::cout << "[WORLD] Updating entity " << entityAtLast << " index from " << last << " to " << idx << std::endl;
                index[entityAtLast] = idx;
                
                // ✅ CRITICAL: If this was the player, update player reference
                if (entityAtLast == player) {
                    std::cout << "[WORLD] Updated player index to " << idx << std::endl;
                }
            }
        }
        
        // Pop last element
        pos.pop_back();
        velocity.pop_back();
        collider.pop_back();
        input.pop_back();
        sprite.pop_back();
        health.pop_back();
        ai.pop_back();
        skill.pop_back();
        infected.pop_back();
    }
    
    std::cout << "[WORLD] Cleanup complete. Remaining entities: " << index.size() << std::endl;
    
    // ✅ Validate player index after cleanup
    if (player != INVALID) {
        auto playerIt = index.find(player);
        if (playerIt != index.end()) {
            size_t p = playerIt->second;
            std::cout << "[WORLD] Player index validation: " << p << " (max: " << pos.size() << ")" << std::endl;
            
            if (p >= pos.size()) {
                std::cout << "[WORLD] CRITICAL ERROR: Player index out of bounds!" << std::endl;
                player = INVALID;
            }
        } else {
            std::cout << "[WORLD] CRITICAL ERROR: Player not in index!" << std::endl;
            player = INVALID;
        }
    }
}

void World::Cleanup() {
    std::cout << "[WORLD] ===== CLEANUP START =====" << std::endl;
    std::cout << "[WORLD] Current entities: " << index.size() << std::endl;
    
    // Delete skills
    for (int i = 0; i < 4; i++) {
        if (playerSkills[i] != nullptr) {
            std::cout << "[WORLD] Deleting skill " << i << std::endl;
            delete playerSkills[i];
            playerSkills[i] = nullptr;
        }
    }
    
    std::cout << "[WORLD] Clearing vectors..." << std::endl;
    pos.clear();
    velocity.clear();
    collider.clear();
    input.clear();
    sprite.clear();
    health.clear();
    ai.clear();
    skill.clear();
    infected.clear();
    index.clear();
    fragments.clear();
    
    player = INVALID;
    next_id = 1;
    dogPosition = {0, 0};
    
    std::cout << "[WORLD] ===== CLEANUP COMPLETE =====" << std::endl;
}

World::~World() {
    std::cout << "[WORLD] ===== DESTRUCTOR CALLED =====" << std::endl;
    
    bool alreadyClean = true;
    for (int i = 0; i < 4; i++) {
        if (playerSkills[i] != nullptr) {
            alreadyClean = false;
            break;
        }
    }
    
    if (alreadyClean) {
        std::cout << "[WORLD] Already cleaned, skipping" << std::endl;
    } else {
        Cleanup();
    }
    
    std::cout << "[WORLD] ===== DESTRUCTOR COMPLETE =====" << std::endl;
}

size_t World::GetEntityCount() const {
    return index.size();
}

size_t World::GetAliveEnemyCount() const {
    size_t count = 0;
    for (const auto& pair : index) {
        if (pair.first == player) continue;
        
        size_t idx = pair.second;
        if (idx < ai.size() && ai[idx].type == 999) {
            if (idx < health.size() && health[idx].hp > 0) {
                count++;
            }
        }
    }
    return count;
}

bool World::IsEntityValid(Entity e) const {
    auto it = index.find(e);
    if (it == index.end()) return false;
    
    size_t idx = it->second;
    if (idx >= ai.size()) return true;
    
    return ai[idx].type != -1 && ai[idx].type != -2;
}

void World::PrintEntityInfo() const {
    std::cout << "\n=== WORLD ENTITY INFO ===" << std::endl;
    std::cout << "Total entities: " << index.size() << std::endl;
    std::cout << "Player entity: " << player << std::endl;
    
    for (const auto& pair : index) {
        Entity e = pair.first;
        size_t idx = pair.second;
        
        std::cout << "Entity " << e << " [idx: " << idx << "]: ";
        
        if (idx < ai.size()) {
            std::cout << "AI type=" << ai[idx].type;
            
            if (ai[idx].type == -1) std::cout << " (DESTROYED)";
            else if (ai[idx].type == -2) std::cout << " (CLEANED UP)";
            else if (ai[idx].type == 999) std::cout << " (ENEMY)";
            else if (e == player) std::cout << " (PLAYER)";
            else std::cout << " (OTHER)";
        }
        
        if (idx < pos.size()) {
            std::cout << " pos=(" << pos[idx].pos.x << "," << pos[idx].pos.y << ")";
        }
        
        if (idx < health.size()) {
            std::cout << " hp=" << health[idx].hp << "/" << health[idx].max_hp;
        }
        
        std::cout << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}