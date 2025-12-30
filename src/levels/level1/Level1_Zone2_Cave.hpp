#pragma once
#include "Level1_ZoneBase.hpp"
#include <vector>

class Level1_Zone2_Cave : public Level1_ZoneBase {
public:
    void Load(World& world) override;
    void Unload(World& world) override;
    void Update(World& world, float dt) override;
    void Render() override;
    
private:
    std::vector<GasVent> vents;
    std::vector<Entity> enemies;
    int puzzleButtons = 0;
};