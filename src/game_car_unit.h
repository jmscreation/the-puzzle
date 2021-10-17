#pragma once

#include "car_entity.h"
#include "game_controller.h"
#include "map.h"

#include <vector>

class GameCar : public Car {

    const std::vector<MapPoint>& pointList;

    Clock interval;

public:
    GameCar(float x, float y);
    virtual ~GameCar();

    bool update(float delta) override;
};