#include "Level1_Zone6_Exit.hpp"
#include "../../utills/Stubs.hpp"
#include <iostream>

void Level1_Zone6_Exit::Load(World& world) {
    std::cout << "[ZONE 6] Loading Exit Zone..." << std::endl;
    bounds = {2700, 0, 1000, 1000};
}

void Level1_Zone6_Exit::Unload(World& world) {
    std::cout << "[ZONE 6] Unloading Exit Zone..." << std::endl;
}

void Level1_Zone6_Exit::Update(World& world, float dt) {
    if (world.player == INVALID) return;
    if (!world.index.count(world.player)) return;
    
    size_t p = world.index[world.player];
    if (p >= world.pos.size()) return;
    
    Vector2 pos = world.pos[p].pos;
    Rectangle exitArea = {bounds.x + bounds.width - 50, 180, 50, 40};
    
    if (CheckCollisionPointRec(pos, exitArea)) {
        std::cout << "[ZONE 6] Player reached exit! Level complete!" << std::endl;
    }
}

void Level1_Zone6_Exit::Render() {
    // Peaceful zone background
    DrawRectangleGradientV(
        (int)bounds.x, (int)bounds.y,
        (int)bounds.width, (int)bounds.height,
        (Color){40, 60, 90, 255},
        (Color){30, 40, 60, 255}
    );
    
    // Draw path to exit
    DrawRectangle((int)bounds.x, (int)(bounds.y + bounds.height - 30),
                 (int)bounds.width, 30, (Color){120, 100, 80, 255});
    
    // Draw exit gate
    Rectangle exitGate = {bounds.x + bounds.width - 50, 180, 50, 40};
    DrawRectangleRec(exitGate, (Color){200, 180, 120, 255});
    DrawRectangleLinesEx(exitGate, 3.0f, GOLD);
    
    DrawText("EXIT",
            (int)(exitGate.x + 5),
            (int)(exitGate.y + 10),
            20, BLACK);
    
    // Zone label
    DrawText("CONG RA",
            (int)(bounds.x + bounds.width/2 - 50),
            (int)bounds.y + 20,
            22, (Color){180, 160, 140, 255});
}