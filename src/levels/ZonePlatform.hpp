// src/levels/ZonePlatform.hpp
#pragma once
#include <raylib.h>

// Simple Platform struct riêng cho zones, KHÔNG dùng PhysicsSystem
struct ZonePlatform {
    Rectangle bounds;
    float friction;
    
    ZonePlatform() : bounds{0,0,0,0}, friction(1.0f) {}
    ZonePlatform(Rectangle r, float f = 1.0f) : bounds(r), friction(f) {}
};