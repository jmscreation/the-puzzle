#include "debug_point.h"

olc::PixelGameEngine* DebugPoint::pge = nullptr;

DebugPoint::DebugPoint(float x, float y, int width, int height, olc::Pixel color):
    sprite(new olc::Sprite(width, height)),
    texture(sprite), x(x), y(y), width(width), height(height), color(color) {

    Init();
}

DebugPoint::DebugPoint():
    sprite(new olc::Sprite(8, 8)), texture(sprite), x(0), y(0), width(8), height(8), color(olc::WHITE) {

    Init();
}

DebugPoint::~DebugPoint() {
    delete sprite;
}

void DebugPoint::Init() {
    pge->SetDrawTarget(sprite);
    pge->Clear(olc::BLANK);
    pge->DrawCircle(width/2,height/2,std::min(floor(width/4)*2 - 1, floor(height/4)*2 - 1));
    pge->SetDrawTarget(nullptr);

    texture.Update();
}

void DebugPoint::draw() {
    if(pge != nullptr) pge->DrawDecal({x - width / 2, y - height / 2}, &texture, {1.0f,1.0f}, color);
}