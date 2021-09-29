#include "game_controller.h"

Controller::Controller(olc::PixelGameEngine& pge): pge(pge), mousePoint(0,0) {
    generate_interval = 1;

    mousePoint.color = olc::GREEN;

    gameMap.LoadMap("default.map");

    place = new Road(0, 0, 0, 0);
    place->blend = olc::Pixel(255, 255, 255, 180);
    place->setDepth(9.f);
}

Controller::~Controller() {
    place->destroyMe();
}

void Controller::update(float delta) {

    if(generate_timer.getSeconds() > generate_interval){
        generate_interval = float(3 + rand() % 20) / 10.f;
        generate_timer.restart();
    }


    // Road Placement Code
    int32_t wheel = pge.GetMouseWheel();
    if(pge.GetMouseWheel() != 0){
        if(pge.GetKey(olc::CTRL).bHeld){
            placeType = (placeType + (wheel > 0 ? 1 : -1));
            placeType = (placeType < 0 ? 8 : placeType) % 9;
        } else {
            placeDir = (placeDir + (wheel > 0 ? 1 : -1));
            placeDir = (placeDir < 0 ? 3 : placeDir) % 4;
        }
    }

    auto mpos = pge.GetMousePos();
    int cellx = floor(mpos.x / Road::width), celly = floor(mpos.y / Road::height);
    bool leftClick = pge.GetMouse(olc::Mouse::LEFT).bPressed,
         rightClick = pge.GetMouse(olc::Mouse::RIGHT).bPressed;
    if(leftClick ^ rightClick){
        Entity* selected = nullptr;
        for(Entity* e : Entity::entities){
            if(dynamic_cast<Road*>(e) == nullptr || e == place) continue;
            int ex = floor(e->position.x / Road::width), ey = floor(e->position.y / Road::height);
            if(ex == cellx && ey == celly){
                selected = e;
                break;
            }
        }

        if(leftClick) new Road(floor(mpos.x / Road::width), floor(mpos.y / Road::height), placeDir, placeType);
        if(selected != nullptr) selected->destroyMe();
    }

    place->updatePlacement(cellx, celly, placeDir, placeType);

}

void Controller::updateAfter(float delta) {
    mousePoint.x = pge.GetMousePos().x;
    mousePoint.y = pge.GetMousePos().y;
    mousePoint.draw();

    for(const MapPoint& p :gameMap.GetMapPoints()){
        p.dbgpoint->draw();
    }
}