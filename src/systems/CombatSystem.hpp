#pragma once
#include "../core/World.hpp"

namespace CombatSystem {
    void Init(World& world);
    void Update(World& world, float dt);
    void Render(const World& world);
}