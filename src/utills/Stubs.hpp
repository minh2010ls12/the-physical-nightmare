#pragma once
#include <raylib.h>
#include <bits/stdc++.h>

// TEMPORARY STUBS - Will be implemented later
namespace SoundManager {
    inline void Play(const std::string& filename) {
        (void)filename; // Suppress warning
        // TODO: Implement sound system
    }
    
    inline void Load(const std::string& key, const std::string& path) {
        (void)key;
        (void)path;
        // TODO: Load sound from file
    }
    
    inline void SetVolume(float volume) {
        (void)volume;
        // TODO: Set global volume
    }
}

namespace ParticleSystem {
    inline void EmitBlood(Vector2 pos, int count, Color color) {
        // TODO: Implement blood particles
        for (int i = 0; i < count; i++) {
            float angle = (float)i / count * 360.0f * DEG2RAD;
            float dist = 20.0f + (float)(i % 5) * 5.0f;
            DrawCircle(pos.x + cosf(angle) * dist, 
                      pos.y + sinf(angle) * dist, 
                      3, color);
        }
    }
    
    inline void EmitTrail(Vector2 pos, Vector2 vel, int count, Color color) {
        // TODO: Implement trail particles
        for (int i = 1; i <= count; i++) {
            float alpha = 1.0f - (float)i / count;
            DrawCircle(pos.x - vel.x * 0.01f * i, 
                      pos.y - vel.y * 0.01f * i, 
                      5, ColorAlpha(color, alpha));
        }
    }
    
    inline void EmitDeath(Vector2 pos, int count, Color color) {
        // Death explosion effect
        for (int i = 0; i < count; i++) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            float dist = (float)GetRandomValue(10, 50);
            float size = (float)GetRandomValue(3, 8);
            DrawCircle(pos.x + cosf(angle) * dist,
                      pos.y + sinf(angle) * dist,
                      size, ColorAlpha(color, 0.7f));
        }
    }
    
    inline void EmitSpark(Vector2 pos, Color color) {
        // Simple spark
        DrawCircle(pos.x, pos.y, 3, color);
        DrawCircle(pos.x, pos.y, 6, ColorAlpha(color, 0.5f));
    }
}

inline void CameraShake(float duration, float intensity) {
    (void)duration;
    (void)intensity;
    // TODO: Implement camera shake
}

inline float GetRandomValueFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}