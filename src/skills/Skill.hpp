#pragma once
#include "../core/World.hpp"
#include <raylib.h>
#include <raymath.h>
class Skill {
public:
    virtual ~Skill() = default;
    virtual void Activate(World& w, size_t playerIdx) = 0;
    virtual void Update(World& w, size_t playerIdx, float dt) = 0;
    virtual void Render(Vector2 playerPos, float facing) = 0;
};