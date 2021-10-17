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

    // Save Map - temporary
    if(pge.GetKey(olc::S).bPressed && pge.GetKey(olc::CTRL).bHeld){
        std::string path;
        freedialog::SaveDialog dlg(path);

        if(path.empty() || !gameMap.SaveMap(path)){
            freedialog::MessageDialog msg("Error saving map!", "Error", MB_ICONERROR);
        }
    }

    // Load Map - temporary
    if(pge.GetKey(olc::L).bPressed && pge.GetKey(olc::CTRL).bHeld){
        std::string path;
        freedialog::LoadDialog dlg(path);

        if(path.empty() || !gameMap.LoadMapFile(path)){
            freedialog::MessageDialog msg("Error loading map!", "Error", MB_ICONERROR);
        }
    }

    auto mpos = pge.GetMousePos();
    bool leftClick = pge.GetMouse(olc::Mouse::LEFT).bPressed,
         rightClick = pge.GetMouse(olc::Mouse::RIGHT).bPressed;

    if(!pge.GetKey(olc::SPACE).bHeld){
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

        int cellx = floor(mpos.x / Road::width), celly = floor(mpos.y / Road::height);
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
        mousePoint.x = -32;
    } else {

        if(leftClick){
            gameMap.AddMapPoint(mpos.x, mpos.y);
        }

        mousePoint.x = mpos.x;
        mousePoint.y = mpos.y;
        place->updatePlacement(-1, -1, 0, 0);
    }

}

void Controller::updateAfter(float delta) {
    mousePoint.draw();

    for(const MapPoint& p :gameMap.GetMapPoints()){
        p.dbgpoint->draw();
    }
}