#include "car_entity.h"

Car::Car(float x, float y): Entity(EntityTypes.car_png_strip),
    direction(0), to_direction(0), speed(0), to_speed(0), turn_speed(8),
    accelleration(20.f), decelleration(80.f), max_speed(96.f),
    testPointA(0,0, 16,16, olc::BLUE),
    testPointB(0,0, 16,16, olc::GREEN),
    testPointC(0,0, 16,16, olc::YELLOW),
    testPointD(0,0, 16,16, olc::CYAN),
    testPointE(0,0, 16,16, olc::MAGENTA)

    {
    position.x = x;
    position.y = y;
    origin = frameLocation.size / 2.f;
    setFrame(rand());
}


Car::~Car() {
}

bool Car::update(float delta) {
    { // modulate direction within a 360° circle and keep speed within range
        to_direction = fmod(to_direction + 360.f, 360.f);
        direction = fmod(direction + 360.f, 360.f);
        to_speed = std::clamp(to_speed, 0.f, max_speed);
    }

    { // steering toward to_direction apply to direction
        float diff = fabs(to_direction - direction),
              dist = diff > 180.f ? 360.f - diff : diff,
              sign = fmod(to_direction - direction + 360.f, 360.f) < 180.f ? 1.f : -1.f,
              ddelta = sign * delta * std::clamp(dist / 64.f, 0.f, 1.f) * std::clamp(speed / 4.f, 0.f, 15.f);

        direction += ddelta * turn_speed;
    }

    { // accelleration / braking toward to_speed apply to speed
        bool gas = to_speed > speed;
        float dist = fabs(to_speed - speed),
              adelta = float(gas ? 1 : -1) * delta * std::clamp(dist / 2.f, 0.f, 1.f);

        speed += adelta * (gas ? accelleration : decelleration);
    }


    { // move instance position
        rotation = (90 - direction) / 180.f * PI;
        olc::vf2d vec = {cosf(direction / 180.f * PI), -sinf(direction / 180.f * PI)};

        position += vec * delta * speed;
    }

    olc::vi2d mpos = pge->GetMousePos();

    if(position.y < -100 || position.x < -100
       || position.x > pge->ScreenWidth() + 100 || position.y > pge->ScreenHeight() + 100
       || (pge->GetMouse(olc::Mouse::RIGHT).bPressed && checkCollision(mpos.x, mpos.y))){
        return false; // destroy if outside or right clicked on
    }

    return true;
}


void Car::draw(float delta) {
    Entity::draw(delta); // inherited event
    
    olc::vi2d mpos = pge->GetMousePos();
    
    /*
    testPointA.draw();
    testPointB.draw();
    testPointC.draw();
    testPointD.draw();
    testPointE.draw();
    */
}

bool Car::checkCollision(float xx, float yy) {

    float sn = sin(-rotation), cs = cos(-rotation),
            vx_h = cs, vy_h = -sn,
            vx_v = sn, vy_v = cs,
            px = xx - position.x, py = yy - position.y;
    // dot product A
    float dist_h = fabs(px * vx_h + py * vy_h),
    // dot product B
          dist_v = fabs(px * vx_v + py * vy_v);
/*
    testPointA.x = position.x;
    testPointA.y = position.y;

    testPointB.x = position.x + vx_h * frameLocation.size.x * scale.x / 2.f;
    testPointB.y = position.y + vy_h * frameLocation.size.x * scale.x / 2.f;

    testPointC.x = position.x + vx_v * frameLocation.size.y * scale.y / 2.f;
    testPointC.y = position.y + vy_v * frameLocation.size.y * scale.y / 2.f;

    testPointD.x = position.x + vx_h * (px * vx_h + py * vy_h); //dist_h (no abs)
    testPointD.y = position.y + vy_h * (px * vx_h + py * vy_h); //dist_h (no abs)

    testPointE.x = position.x + vx_v * (px * vx_v + py * vy_v); //dist_v (no abs)
    testPointE.y = position.y + vy_v * (px * vx_v + py * vy_v); //dist_v (no abs)
*/

    return (dist_h < (frameLocation.size.x - origin.x)*scale.x && dist_v < (frameLocation.size.y - origin.y)*scale.y);
}

Car* Car::checkAllCollision(float x, float y, Car* me) {
    for(Entity* e : entities){
        Car* c = dynamic_cast<Car*>(e);
        if(c == nullptr || c == me) continue;

        if(c->checkCollision(x, y)) return c;
    }
    return nullptr;
}

void Car::clearCars() {
    for(Entity* e : entities){
        if(dynamic_cast<Car*>(e) == nullptr) continue;

        e->destroyMe();
    }
    Entity::EntityUpdate(0.0f); // force update for immediate clean
}