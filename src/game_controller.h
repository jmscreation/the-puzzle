#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"
#include "entity.h"
#include "car_entity.h"
#include "debug_point.h"

#include <vector>

class Controller {
    Clock generate_timer;

    float generate_interval; // seconds to generate new car

    olc::PixelGameEngine& pge;

    DebugPoint mousePoint;

public:
    Controller(olc::PixelGameEngine& pge);
    virtual ~Controller();

    void update(float delta);
    void updateAfter(float delta);
};