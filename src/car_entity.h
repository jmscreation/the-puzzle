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
    float speed, turn_speed;

    Clock turnTimer;

    // debugging
    olc::Sprite *spr;
    olc::Decal *collisionBoundary;

    DebugPoint testPointA, testPointB, testPointC, testPointD, testPointE;

    bool collide;

protected:
    virtual ~Car();

public:
    Car(float x, float y);

    virtual bool update(float delta) override;
    virtual void draw(float delta) override;
};