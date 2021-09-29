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

    // debugging

    DebugPoint testPointA, testPointB, testPointC, testPointD, testPointE;

protected:
    virtual ~Car();

public:
    Car(float x, float y);

    virtual bool update(float delta) override;
    virtual void draw(float delta) override;


    bool checkCollision(float x, float y);

    static Car* checkAllCollision(float x, float y, Car* me=nullptr);
    static void clearCars();
};