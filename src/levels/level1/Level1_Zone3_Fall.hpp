#pragma once
#include "Level1_ZoneBase.hpp"
#include <vector>

class Level1_Zone3_Fall : public Level1_ZoneBase {
public:
    void Load(World& world) override;
    void Unload(World& world) override;
    void Update(World& world, float dt) override;
    void Render() override;
    
private:
    std::vector<Rectangle> fallingRocks;
};