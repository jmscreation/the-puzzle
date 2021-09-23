#include "game.h"

Window::Window(): manager(globalAssetManager), controller(nullptr)  {
    sAppName = "Simple Puzzle Game";
}

Window::~Window(){}

bool Window::OnUserCreate() {
    bool success = true;

    success &= Entity::LoadNewEntityAnimation("test.ani", EntityTypes.test_animation);
    success = success && Entity::LoadNewEntityAnimation("cars.ani", EntityTypes.car_png_strip);
    success = success && Entity::LoadNewEntityAnimation("roads.ani", EntityTypes.road_png_strip);

    if(!success){
        std::cout << "failed to load resources\n";
        return false;
    }
    
    DebugPoint::pge = this;
    controller = new Controller(*(olc::PixelGameEngine*)this);

    return success; // whether sprites loaded or not
}


bool Window::OnUserUpdate(float delta) {
    const float width = 128, height = 128;
    Clear(olc::BLANK);

    controller->update(delta); // controller begin step

    Entity::EntityUpdate(delta);

    controller->updateAfter(delta); // controller end step
    
    return true;
}

bool Window::OnUserDestroy() {

    delete controller;
    controller = nullptr;
    
    Entity::UnloadResources(); // cleanup everything

    return true;
}