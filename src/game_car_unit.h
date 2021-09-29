#pragma once

#include "car_entity.h"

class GameCar : public Car {
public:
    GameCar(float x, float y);
    virtual ~GameCar();

    bool update(float delta) override;
};