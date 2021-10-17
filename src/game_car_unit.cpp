#include "game_car_unit.h"

GameCar::GameCar(float x, float y): Car(x, y) {

}

GameCar::~GameCar() {

}

bool GameCar::update(float delta) {
    Car::update(delta); // inherit

    return true;    
}