// src/levels/level1/Level1_Newton.cpp
#include "Level1_Newton.hpp"
#include "Level1_Zone0_Spawn.hpp"
#include "Level1_Zone1_Slope.hpp"
#include "Level1_Zone2_Cave.hpp"
#include "Level1_Zone3_Fall.hpp"
#include "Level1_Zone4_Hidden.hpp"  // ✅ FIX #9: Correct filename
#include "Level1_Zone5_Boss.hpp"
#include "Level1_Zone6_Exit.hpp"
#include "../../utills/Stubs.hpp"
#include <memory>
#include <iostream>

void Level1_Newton::Load(World& world) {
    std::cout << "[LEVEL 1] Loading Newton's Valley..." << std::endl;
    
    mainZones.clear();
    mainZones.emplace_back(std::make_unique<Level1_Zone0_Spawn>());
    mainZones.emplace_back(std::make_unique<Level1_Zone1_Slope>());
    mainZones.emplace_back(std::make_unique<Level1_Zone2_Cave>());
    mainZones.emplace_back(std::make_unique<Level1_Zone3_Fall>());
    mainZones.emplace_back(std::make_unique<Level1_Zone5_Boss>());
    mainZones.emplace_back(std::make_unique<Level1_Zone6_Exit>());
    
    // ✅ FIX #9: Correct class name
    hiddenZone = std::make_unique<Level1_Zone4_Hidden>();
    
    for (auto& zone : mainZones) {
        zone->Load(world);
    }
    hiddenZone->Load(world);
    
    currentMainIdx = 0;
    std::cout << "[LEVEL 1] Loaded successfully!" << std::endl;
}

void Level1_Newton::Update(World& world, float dt) {
    for (auto& zone : mainZones) {
        zone->Update(world, dt);
    }
    hiddenZone->Update(world, dt);
    
    if (world.player == INVALID || world.index.find(world.player) == world.index.end()) {
        return;
    }
    
    size_t p = world.index[world.player];
    if (p >= world.pos.size()) return;
    
    Vector2 pos = world.pos[p].pos;
    
    Rectangle hiddenEntrance = {3800, 4500, 200, 200};
    if (CheckCollisionPointRec(pos, hiddenEntrance)) {
        if (world.fragments.size() >= 3) {
            std::cout << "[LEVEL 1] Hidden zone unlocked!" << std::endl;
            SoundManager::Play("unlock.wav");
        }
    }
}

void Level1_Newton::Render() {
    for (auto& zone : mainZones) {
        zone->Render();
    }
    hiddenZone->Render();
}

void Level1_Newton::Unload(World& world) {
    std::cout << "[LEVEL 1] Unloading..." << std::endl;
    
    for (auto& z : mainZones) {
        z->Unload(world);
    }
    
    if (hiddenZone) {
        hiddenZone->Unload(world);
    }
    
    mainZones.clear();
    hiddenZone.reset();
}