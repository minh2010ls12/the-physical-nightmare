// src/levels/level1/Level1_Newton.hpp
#pragma once
#include "../../core/World.hpp"
#include "Level1_ZoneBase.hpp"
#include <vector>
#include <memory>

// Forward declarations
class Level1_Zone0_Spawn;
class Level1_Zone1_Slope;
class Level1_Zone2_Cave;
class Level1_Zone3_Fall;
class Level1_Zone4_Hidden;  // ✅ FIX #9: Correct class name
class Level1_Zone5_Boss;
class Level1_Zone6_Exit;

class Level1_Newton {
public:
    void Load(World& world);
    void Unload(World& world);
    void Update(World& world, float dt);
    void Render();
    
    // ✅ Allow RenderSystem to access zones
    std::vector<std::unique_ptr<Level1_ZoneBase>> mainZones;
    std::unique_ptr<Level1_ZoneBase> hiddenZone;
    
private:
    int currentMainIdx = 0;
};