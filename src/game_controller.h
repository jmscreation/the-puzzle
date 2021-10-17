#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"
#include "entity.h"
#include "road_entity.h"
#include "debug_point.h"
#include "map.h"
#include "freedialog.h"

#include "game_car_unit.h"
#include "car_entity.h"

#include <vector>

class Controller {
    static Controller* curInstance;

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

    static inline Controller* currentController() { return curInstance; }

    inline Map& getMap() { return gameMap; }

    void update(float delta);
    void updateAfter(float delta);
};