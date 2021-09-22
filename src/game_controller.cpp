#include "game_controller.h"

Controller::Controller(olc::PixelGameEngine& pge): pge(pge), mousePoint(0,0) {
    generate_interval = 1;

    Entity* e = new Entity(EntityTypes.test_animation);
    e->position = {pge.ScreenWidth() / 2.0f - 64, pge.ScreenHeight() / 2.0f - 64};
    e->origin = e->frameLocation.size / 2.0f;
    e->scale = {0.1f, 0.1f};

    mousePoint.color = olc::GREEN;
}

Controller::~Controller() {
}

void Controller::update(float delta) {

    if(generate_timer.getSeconds() > generate_interval){
        Car* c = new Car(rand() % pge.ScreenWidth(), rand() % pge.ScreenHeight());
        c->to_direction = rand() % 360;
        c->speed = 8 + rand() % 32;
        c->turn_speed = float(5 + rand()%395) / 10.f;

        generate_interval = 12 + rand() % 5;
        generate_timer.restart();
    }

}

void Controller::updateAfter(float delta) {
    mousePoint.x = pge.GetMousePos().x;
    mousePoint.y = pge.GetMousePos().y;
    mousePoint.draw();
}