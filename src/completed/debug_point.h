#pragma once

#include "olcPixelGameEngine.h"

class DebugPoint {
    void Init();
public:
    static olc::PixelGameEngine* pge;

    olc::Sprite* sprite;
    olc::Decal texture;
    float x, y;
    const int width, height;
    olc::Pixel color;
public:

    DebugPoint(float x, float y, int width=8, int height=8, olc::Pixel color=olc::WHITE);
    DebugPoint();

    virtual ~DebugPoint();
    
    void draw();
};