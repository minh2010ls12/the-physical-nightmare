// src/levels/level1/Level1_Zone0_Spawn.hpp
#pragma once
#include "Level1_ZoneBase.hpp"
#include <vector>
#include <string>

class Level1_Zone0_Spawn : public Level1_ZoneBase {
private:
    // ✅ FIX #7: Remove "struct" keyword - use TutorialTrigger from ZoneBase
    std::vector<TutorialTrigger> tutorialTriggers;
    Vector2 windForce = {0.2f, 0.0f};
    Rectangle windArea = {0, 0, 480, 200};
    
    struct CrackParticle {
        Vector2 position;
        Vector2 velocity;
        float lifetime;
    };
    std::vector<CrackParticle> crackParticles;
    
    void CreateCrackParticles();
    void UpdateCrackParticles(float dt);
    void RenderCrackParticles();

public:
    Level1_Zone0_Spawn();
    void Load(World& world) override;
    void Unload(World& world) override;
    void Update(World& world, float dt) override;
    void Render() override;
    Vector2 GetWindForce(const Vector2& position) override;
    bool CheckCollision(const Rectangle& rect, Vector2* correction = nullptr) override;
};