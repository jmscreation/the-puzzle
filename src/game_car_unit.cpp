#include "game_car_unit.h"

GameCar::GameCar(float x, float y): Car(x, y), pointList(Controller::currentController()->getMap().GetMapPoints()) {

}

GameCar::~GameCar() {

}

bool GameCar::update(float delta) {
    Car::update(delta); // inherit

    Controller& cont = *Controller::currentController();
    
    if(pointList.size() && interval.getSeconds() > 8){
        const MapPoint* selected = nullptr;
        do {
            for(auto& point : pointList){
                if(!(rand() % pointList.size())) selected = &point;
            }
        } while(selected == nullptr);

        position.x = selected->pos.x;
        position.y = selected->pos.y;

        interval.restart();
    }

    return true;    
}