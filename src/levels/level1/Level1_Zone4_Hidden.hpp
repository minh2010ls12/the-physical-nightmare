#pragma once
#include "Level1_ZoneBase.hpp"
#include <vector>

struct Chest {
    Rectangle bounds;
    int skillIndex;
    bool opened;
};

class Level1_Zone4_Hidden : public Level1_ZoneBase {
public:
    void Load(World& world) override;
    void Unload(World& world) override;
    void Update(World& world, float dt) override;
    void Render() override;
    
private:
    std::vector<Chest> skillChests;
    Rectangle entrance;
    bool isUnlocked = false;
    bool dogFound = false;
    
    // ✅ FIX: Add missing members
    Vector2 dogPosition = {0, 0};
    float flickerTimer = 0.0f;
    std::vector<int> fragments;  // Track fragments locally
};