#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"
#include "entity.h"
#include "car_entity.h"
#include "road_entity.h"
#include "debug_point.h"
#include "map.h"
#include "freedialog.h"

#include <vector>

class Controller {
    Clock generate_timer;

    float generate_interval; // seconds to generate new car

    olc::PixelGameEngine& pge;

    Map gameMap;

    DebugPoint mousePoint;
    Road* place;
    int placeType;
    char placeDir;

public:
    Controller(olc::PixelGameEngine& pge);
    virtual ~Controller();

    void update(float delta);
    void updateAfter(float delta);
};