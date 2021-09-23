#include "game_controller.h"

Controller::Controller(olc::PixelGameEngine& pge): pge(pge), mousePoint(0,0) {
    generate_interval = 1;

    mousePoint.color = olc::GREEN;

    place = new Road(0, 0, 0, 0);
    place->blend = olc::Pixel(255, 255, 255, 180);
    place->setDepth(9.f);
}

Controller::~Controller() {
    place->destroyMe();
}

void Controller::update(float delta) {

    if(generate_timer.getSeconds() > generate_interval){
        Car* c = new Car(0, 0);
        c->direction = rand() % 2 ? 45 : 135;
        c->to_direction = 45 + rand() % 90;
        c->to_speed = 24 + rand() % 70;
        c->speed = 10;
        c->inHurry = !(rand() % 3);

        if(rand() % 5){
            c->position.x = rand() % pge.ScreenWidth();
            c->position.y = pge.ScreenHeight() + 64 + rand()%30;

            c->destX = rand()%4 ? c->position.x : rand() % pge.ScreenWidth();
            c->destY = -128;
        } else {
            c->position.x = -64 - rand()%30;
            c->position.y = rand() % pge.ScreenHeight();

            c->destX = pge.ScreenWidth() + 128;
            c->destY = rand()%4 ? c->position.y : rand() % pge.ScreenHeight();
        }

        srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
        generate_interval = float(3 + rand() % 20) / 10.f;
        generate_timer.restart();
    }


    // Road Placement Code
    int32_t wheel = pge.GetMouseWheel();
    if(pge.GetMouseWheel() != 0){
        if(pge.GetKey(olc::CTRL).bHeld){
            placeType = (placeType + (std::signbit(wheel) ? 1 : -1));
            placeType = (placeType < 0 ? 8 : placeType) % 9;
        } else {
            placeDir = (placeDir + (std::signbit(wheel) ? 1 : -1));
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


}