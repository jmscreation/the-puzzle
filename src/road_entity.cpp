#include "road_entity.h"

float Road::width;
float Road::height;

Road::Road(int cellx, int celly, char dir, int type): Entity(EntityTypes.road_png_strip) {
    setDepth(10.f);
    scale /= 2.234636871508379888268156424581; // makes it 128 x 128 in final size
    origin = frameLocation.size / 2.f;

    width = frameLocation.size.x * scale.x;
    height = frameLocation.size.y * scale.y;

    position.x = width - origin.x*scale.x + cellx * width;
    position.y = height - origin.y*scale.y + celly * height;
    rotation = PI/2.f * std::clamp(int(dir), 0, 3);

    setFrame(type);

};

Road::~Road() {

}

bool Road::update(float delta) {


    return true;
}