#pragma once
#include "Level1_ZoneBase.hpp"
#include <vector>

struct SlopeSegment {
    float x1, y1, x2, y2;
    float angle;
    float friction;
};

class Level1_Zone1_Slope : public Level1_ZoneBase {
private:
    std::vector<SlopeSegment> slopeSegments;
    std::vector<Entity> inertiaEnemies;
    
    void CreateSlopeSegments();
    void CreateInertiaEnemies(World& world);

public:
    void Load(World& world) override;
    void Unload(World& world) override;
    void Update(World& world, float dt) override;
    void Render() override;
    
    float GetYOnSlope(float x) const;
    float GetFriction(float x, float y) const;
    bool CheckSlopeCollision(const Rectangle& rect, Vector2* correction = nullptr) const;
};
