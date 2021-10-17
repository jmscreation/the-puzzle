#include "road_entity.h"

float Road::width;
float Road::height;

Road::Road(int cellx, int celly, char dir, int type): Entity(EntityTypes.road_png_strip) {
    setDepth(10.f);
    scale /= 2.234636871508379888268156424581; // makes it 128 x 128 in final size
    origin = frameLocation.size / 2.f;

    width = frameLocation.size.x * scale.x;
    height = frameLocation.size.y * scale.y;

    updatePlacement(cellx, celly, dir, type);

};

Road::~Road() {

}

bool Road::update(float delta) {


    return true;
}

void Road::updatePlacement(int cellx, int celly, char dir, int type) {
    position.x = width - origin.x*scale.x + cellx * width;
    position.y = height - origin.y*scale.y + celly * height;
    rotation = PI/2.f * std::clamp(int(dir), 0, 3);
    setFrame(type);
}

void Road::clearRoads() {
    for(Entity* e : entities){
        if(dynamic_cast<Road*>(e) == nullptr) continue;
        
        if(e->blend.a > 250) // a bit of a hack to prevent placement roads from being killed
            e->destroyMe();
    }
    Entity::EntityUpdate(0.0f); // force update for immediate clean
}