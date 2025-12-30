#include "Game.hpp"
#include "../systems/PlayerSystem.hpp"
#include "../systems/CombatSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../utills/PhysicsSystem.hpp"
#include "../skills/SkillZ_Plasma.hpp"
#include "../skills/SkillX_Shield.hpp"
#include "../skills/SkillC_Bomb.hpp"
#include "../skills/SkillV_Ultimate.hpp"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

void Game::Init() {
    std::cout << "[GAME] Initializing..." << std::endl;
    
    RenderSystem::Init();
    
    world.player = world.Create();
    size_t playerIdx = world.index[world.player];
    
    if (playerIdx < world.health.size()) {
        world.health[playerIdx].hp = 1000;
        world.health[playerIdx].max_hp = 1000;
    }
    
    if (playerIdx < world.pos.size()) {
        world.pos[playerIdx].pos = {2500, 4600};
        world.pos[playerIdx].facing = 1.0f;
    }
    
    if (playerIdx < world.collider.size()) {
        world.collider[playerIdx].rect = {0, 0, 16, 24};
        world.collider[playerIdx].offset = {0, 0};
    }
    
    if (playerIdx < world.sprite.size()) {
        world.sprite[playerIdx].color = PURPLE;
    }
    
    if (playerIdx < world.skill.size()) {
        world.skill[playerIdx].z_unlock = true;
        world.skill[playerIdx].x_unlock = true;
        world.skill[playerIdx].c_unlock = true;
        world.skill[playerIdx].v_unlock = true;
    }
    
    cam.target = {2500, 4600};
    cam.offset = {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
    cam.rotation = 0.0f;
    cam.zoom = 1.5f;
    
    PhysicsSystem::Init();
    
    // ✅ CRITICAL: Create skills BEFORE CombatSystem::Init
    std::cout << "[GAME] Creating player skills..." << std::endl;
    for (int i = 0; i < 4; i++) {
        if (world.playerSkills[i] != nullptr) {
            std::cout << "[GAME] Deleting old skill " << i << std::endl;
            delete world.playerSkills[i];
            world.playerSkills[i] = nullptr;
        }
    }
    
    world.playerSkills[0] = new SkillZ_Plasma();
    world.playerSkills[1] = new SkillX_Shield();
    world.playerSkills[2] = new SkillC_Bomb();
    world.playerSkills[3] = new SkillV_Ultimate();
    std::cout << "[GAME] Skills created" << std::endl;
    
    // Now init combat system (it won't touch skills)
    CombatSystem::Init(world);
    
    std::cout << "[GAME] Loading Level 1: Newton's Valley..." << std::endl;
    currentLevel.Load(world);
    levelLoaded = true;
    
    std::cout << "[GAME] Initialized successfully!" << std::endl;
}

void Game::Update(float dt) {
    PhysicsSystem::Update(world, dt);
    PlayerSystem_Update(world, dt);  
    
    if (levelLoaded) {
        currentLevel.Update(world, dt);
    }
    
    CombatSystem::Update(world, dt);
    
    static int lastScreenWidth = GetScreenWidth();
    static int lastScreenHeight = GetScreenHeight();
    if (GetScreenWidth() != lastScreenWidth || GetScreenHeight() != lastScreenHeight) {
        cam.offset.x = (float)GetScreenWidth() / 2.0f;
        cam.offset.y = (float)GetScreenHeight() / 2.0f;
        lastScreenWidth = GetScreenWidth();
        lastScreenHeight = GetScreenHeight();
    }
    
    if (world.player != INVALID) {
        auto it = world.index.find(world.player);
        if (it != world.index.end()) {
            size_t p = it->second;
            if (p < world.pos.size()) {
                Vector2 targetPos = world.pos[p].pos;
                cam.target.x += (targetPos.x - cam.target.x) * 0.1f;
                cam.target.y += (targetPos.y - cam.target.y) * 0.1f;
                
                cam.zoom = Clamp(cam.zoom, 1.0f, 3.0f);
            }
        }
    }
    
    if (IsKeyPressed(KEY_R)) {
        ResetGame();
    }
    
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        cam.zoom += wheel * 0.2f;
        cam.zoom = Clamp(cam.zoom, 1.0f, 3.0f);
    }
}

void Game::Render() {
    BeginDrawing();
    ClearBackground((Color){15, 10, 25, 255});
    
    RenderSystem::RenderAll(world, cam, currentLevel);
    
    DrawFPS(10, 10);
    EndDrawing();
}

void Game::DrawHUD() {
    if (world.player == INVALID) return;
    
    auto playerIt = world.index.find(world.player);
    if (playerIt == world.index.end()) return;
    
    size_t p = playerIt->second;
    
    if (p < world.health.size()) {
        DrawRectangle(20, 20, 254, 30, BLACK);
        DrawRectangleLines(20, 20, 254, 30, WHITE);
        
        float healthRatio = (float)world.health[p].hp / world.health[p].max_hp;
        healthRatio = Clamp(healthRatio, 0.0f, 1.0f);
        
        Color healthColor;
        if (healthRatio > 0.6f) healthColor = GREEN;
        else if (healthRatio > 0.3f) healthColor = YELLOW;
        else healthColor = RED;
        
        DrawRectangle(22, 22, (int)(250 * healthRatio), 26, healthColor);
        
        DrawText(TextFormat("HP: %d/%d", world.health[p].hp, world.health[p].max_hp),
                30, 25, 22, WHITE);
    }
    
    if (p < world.skill.size()) {
        int yPos = 65;
        const char* skillNames[] = {"Z - Plasma", "X - Shield", "C - Bomb", "V - Ultimate"};
        float cooldowns[] = {
            world.skill[p].cd_z, world.skill[p].cd_x,
            world.skill[p].cd_c, world.skill[p].cd_v
        };
        const float maxCooldowns[] = {25.0f, 25.0f, 42.0f, 120.0f};
        
        for (int i = 0; i < 4; i++) {
            DrawRectangle(20, yPos, 184, 28, BLACK);
            DrawRectangleLines(20, yPos, 184, 28, WHITE);
            
            if (cooldowns[i] < maxCooldowns[i]) {
                float ratio = cooldowns[i] / maxCooldowns[i];
                DrawRectangle(22, yPos + 2, (int)(180 * ratio), 24, BLUE);
            }
            
            DrawText(TextFormat("%s: %.1fs", skillNames[i], cooldowns[i]),
                    25, yPos + 4, 20, WHITE);
            
            yPos += 35;
        }
    }
    
    int screenHeight = GetScreenHeight();
    DrawText("Controls: WASD/Arrows = Move, SPACE = Jump, SHIFT = Dash", 
             20, screenHeight - 40, 18, LIGHTGRAY);
    DrawText("Mouse Left = Attack, Z/X/C/V = Skills, R = Reset, F3 = Debug", 
             20, screenHeight - 20, 18, LIGHTGRAY);
}

void Game::DrawDebugInfo() {
    if (!IsKeyDown(KEY_F3)) return;
    
    int screenWidth = GetScreenWidth();
    
    DrawRectangle(screenWidth - 210, 10, 190, 200, ColorAlpha(BLACK, 0.7f));
    DrawRectangleLines(screenWidth - 210, 10, 190, 200, WHITE);
    
    DrawText(TextFormat("Entities: %d", (int)world.index.size()), 
             screenWidth - 200, 20, 18, GREEN);
    
    if (world.player != INVALID) {
        auto it = world.index.find(world.player);
        if (it != world.index.end()) {
            size_t p = it->second;
            if (p < world.pos.size()) {
                DrawText(TextFormat("Pos: %.0f, %.0f", 
                        world.pos[p].pos.x, world.pos[p].pos.y),
                        screenWidth - 200, 45, 16, YELLOW);
            }
            
            if (p < world.velocity.size()) {
                DrawText(TextFormat("Vel: %.0f, %.0f",
                        world.velocity[p].vel.x, world.velocity[p].vel.y),
                        screenWidth - 200, 65, 16, YELLOW);
            }
            
            if (p < world.input.size()) {
                DrawText(TextFormat("Ground: %s",
                        world.input[p].grounded ? "YES" : "NO"),
                        screenWidth - 200, 85, 16, YELLOW);
                DrawText(TextFormat("Dash: %s",
                        world.input[p].dashing ? "YES" : "NO"),
                        screenWidth - 200, 105, 16, YELLOW);
            }
        }
    }
    
    DrawText(TextFormat("Cam: %.0f, %.0f", cam.target.x, cam.target.y),
            screenWidth - 200, 130, 16, SKYBLUE);
    DrawText(TextFormat("Zoom: %.2fx", cam.zoom), 
            screenWidth - 200, 150, 16, SKYBLUE);
    
    DrawText(TextFormat("Screen: %dx%d", GetScreenWidth(), GetScreenHeight()),
            screenWidth - 200, 170, 14, GRAY);
    
    if (levelLoaded) {
        DrawText("Level 1 Active", screenWidth - 200, 190, 14, LIME);
    }
}

void Game::ResetGame() {
    std::cout << "[GAME] ========== RESET START ==========" << std::endl;
    
    // ✅ STEP 1: Unload level FIRST (this destroys all enemies)
    if (levelLoaded) {
        std::cout << "[GAME] Unloading level..." << std::endl;
        currentLevel.Unload(world);
    }
    
    // ✅ STEP 2: Clean up remaining entities (except player)
    std::cout << "[GAME] Destroying non-player entities..." << std::endl;
    std::vector<Entity> toDestroy;
    for (const auto& pair : world.index) {
        if (pair.first != world.player) {
            toDestroy.push_back(pair.first);
        }
    }
    
    for (Entity e : toDestroy) {
        world.Destroy(e);
    }
    world.CleanupDestroyedEntities();
    
    // ✅ STEP 3: Reset player state
    std::cout << "[GAME] Resetting player..." << std::endl;
    if (world.player != INVALID && world.index.count(world.player)) {
        size_t p = world.index[world.player];
        
        if (p < world.pos.size()) world.pos[p].pos = {2500, 4600};
        if (p < world.velocity.size()) world.velocity[p].vel = {0, 0};
        if (p < world.input.size()) world.input[p] = PlayerInput{};
        if (p < world.health.size()) {
            world.health[p].hp = 1000;
            world.health[p].invincible = false;
        }
        if (p < world.skill.size()) {
            world.skill[p].cd_z = 25.0f;
            world.skill[p].cd_x = 25.0f;
            world.skill[p].cd_c = 42.0f;
            world.skill[p].cd_v = 120.0f;
        }
    }
    
    // ✅ STEP 4: Reset world state
    world.fragments.clear();
    world.dogPosition = {0, 0};
    
    // ✅ STEP 5: Reload level
    std::cout << "[GAME] Reloading level..." << std::endl;
    if (levelLoaded) {
        currentLevel.Load(world);
    }
    
    // ✅ STEP 6: Reset camera
    cam.target = {2500, 4600};
    cam.zoom = 1.5f;
    
    std::cout << "[GAME] ========== RESET COMPLETE ==========" << std::endl;
}