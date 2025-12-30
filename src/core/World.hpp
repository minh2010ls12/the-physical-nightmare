#pragma once
#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>

using Entity = uint32_t;
constexpr Entity INVALID = 0;

class Skill;
class Level1_Newton;

struct Pos { 
    Vector2 pos{200, 400}; 
    float facing = 1.0f; 
};

struct Velocity { 
    Vector2 vel{}; 
};

struct Collider { 
    Rectangle rect{0,0,14,15}; 
    Vector2 offset{1,1}; 
};

struct PlayerInput { 
    float coyote = 0; 
    bool grounded = false; 
    bool can_double = true; 
    bool dashing = false; 
    float dash_time = 0; 
};

struct Sprite { 
    Color color = PURPLE; 
};

struct Health { 
    int hp = 100;
    long long max_hp = 100; 
    long long phsp = 100; 
    bool invincible = false;  
};

struct Infected { 
    long long phsp = 100; 
    long long max_phsp = 100; 
    float regen_rate = 1.0f; 
};

struct EnemyAI { 
    int type = 0; 
    float timer = 0; 
};

struct SkillState { 
    float cd_z = 25.0f, cd_x = 25.0f, cd_c = 42.0f, cd_v = 120.0f; 
    bool z_unlock = true, x_unlock = true, c_unlock = true, v_unlock = true; 
};

class World {
public:
    Entity player = INVALID;

    std::vector<Pos>         pos;
    std::vector<Velocity>    velocity;
    std::vector<Collider>    collider;
    std::vector<PlayerInput> input;
    std::vector<Sprite>      sprite;
    std::vector<Health>      health;
    std::vector<EnemyAI>     ai;
    std::vector<SkillState>  skill;
    std::vector<Infected>    infected;

    std::unordered_map<Entity, size_t> index;
    uint32_t next_id = 1;

    Skill* playerSkills[4] = {nullptr, nullptr, nullptr, nullptr};

    std::vector<int> fragments;
    Vector2 dogPosition = {0, 0};

    Entity Create();
    void Destroy(Entity e);
    void CleanupDestroyedEntities();
    
    size_t GetEntityCount() const;
    size_t GetAliveEnemyCount() const;
    bool IsEntityValid(Entity e) const;
    
    void PrintEntityInfo() const;
    
    // ✅ FIX: Add cleanup method
    void Cleanup();
    
    ~World();
};