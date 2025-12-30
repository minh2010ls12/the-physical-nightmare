#define GRAPHICS_API_OPENGL_ES2
#include "core/Game.hpp"
#include <raylib.h>
#include <iostream>
#include <csignal>

// ✅ Crash handler để debug
void SignalHandler(int signal) {
    std::cout << "\n[CRASH] Signal caught: " << signal << std::endl;
    std::cout << "[CRASH] Game crashed! Check logs above." << std::endl;
    exit(signal);
}

int main() {
    // Install crash handler
    signal(SIGSEGV, SignalHandler);
    signal(SIGABRT, SignalHandler);
    
    std::cout << "[MAIN] Starting The Physical Nightmare..." << std::endl;
    
    const int WIDTH = 1280, HEIGHT = 720;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    
    std::cout << "[MAIN] InitWindow..." << std::endl;
    InitWindow(WIDTH, HEIGHT, "THE PHYSICAL NIGHTMARE - DEBUG BUILD");
    
    std::cout << "[MAIN] InitAudioDevice..." << std::endl;
    InitAudioDevice();
    
    SetTargetFPS(60);

    std::cout << "[MAIN] Creating Game object..." << std::endl;
    Game* game = new Game();
    
    std::cout << "[MAIN] Calling Game::Init()..." << std::endl;
    game->Init();
    
    std::cout << "[MAIN] Entering main loop..." << std::endl;
    int frameCount = 0;
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // Log every 60 frames
        if (frameCount % 60 == 0) {
            std::cout << "[MAIN] Frame " << frameCount << " - dt: " << dt << std::endl;
        }
        
        try {
            game->Update(dt);
            game->Render();
        } catch (const std::exception& e) {
            std::cout << "[MAIN] Exception caught: " << e.what() << std::endl;
            break;
        }
        
        frameCount++;
    }

    std::cout << "[MAIN] Exiting main loop..." << std::endl;
    std::cout << "[MAIN] Deleting game object..." << std::endl;
    
    delete game;
    
    std::cout << "[MAIN] CloseAudioDevice..." << std::endl;
    CloseAudioDevice();
    
    std::cout << "[MAIN] CloseWindow..." << std::endl;
    CloseWindow();
    
    std::cout << "[MAIN] Exit successful" << std::endl;
    return 0;
}