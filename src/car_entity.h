#pragma once

#include "external_defines.h"
#include "olcPixelGameEngine.h"
#include "entity.h"
#include "clock.h"
#include "debug_point.h"

#include <cmath>

class Car : public Entity {
public:
    float direction, to_direction;
    float speed, to_speed, turn_speed, accelleration, decelleration, max_speed;
    float destX, destY;
    
    bool inHurry;

    Clock turnTimer, stuckTimeout, forwardTimer;

    // debugging
    olc::Sprite *spr;
    olc::Decal *collisionBoundary;

    DebugPoint testPointA, testPointB, testPointC, testPointD, testPointE;

    float slowdown;

protected:
    virtual ~Car();

public:
    Car(float x, float y);

    virtual bool update(float delta) override;
    virtual void draw(float delta) override;


    bool checkCollision(float x, float y);
    Car* lookAhead(float& foundDistance, int& side, float distance = 16.f, float step = 2.5f);

    static Car* checkAllCollision(float x, float y, Car* me=nullptr);
};