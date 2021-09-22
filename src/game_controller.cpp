#include "game_controller.h"

Controller::Controller(olc::PixelGameEngine& pge): pge(pge), mousePoint(0,0) {
    generate_interval = 1;

    mousePoint.color = olc::GREEN;
}

Controller::~Controller() {
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

}

void Controller::updateAfter(float delta) {
    mousePoint.x = pge.GetMousePos().x;
    mousePoint.y = pge.GetMousePos().y;
    mousePoint.draw();
}