#include "car_entity.h"

Car::Car(float x, float y): Entity(EntityTypes.car_png_strip),
    direction(0), to_direction(0), speed(0), turn_speed(32),

    testPointA(0,0, 16,16, olc::BLUE),
    testPointB(0,0, 16,16, olc::GREEN),
    testPointC(0,0, 16,16, olc::YELLOW),
    testPointD(0,0, 16,16, olc::CYAN),
    testPointE(0,0, 16,16, olc::MAGENTA)

    {
    position.x = x;
    position.y = y;
    origin = frameLocation.size / 2.f;
    setFrame(rand() % getFrameRegionCount());
    scale *= 2.f;
    
    // Collision Boundary Test
    spr = new olc::Sprite(frameLocation.size.x, frameLocation.size.y);
    pge->SetDrawTarget(spr);
    pge->Clear(olc::BLANK);
    pge->DrawRect(0,0,int(frameLocation.size.x - 1), int(frameLocation.size.y - 1));
    pge->SetDrawTarget(nullptr);
    collisionBoundary = new olc::Decal(spr);


    collide = false;
}


Car::~Car() {
    delete collisionBoundary;
    delete spr;
}

bool Car::update(float delta) {
    to_direction = fmod(to_direction + 360.f, 360.f);
    direction = fmod(direction + 360.f, 360.f);

    {
        float diff = fabs(to_direction - direction),
              dist = diff > 180.f ? 360.f - diff : diff,
              sign = fmod(to_direction - direction + 360.f, 360.f) < 180.f ? 1.f : -1.f,
              ddelta = sign * delta * std::clamp(dist / 64.f, 0.f, 1.f);

        direction += ddelta * turn_speed;
    }

    rotation = (90 - direction) / 180.f * PI;
    olc::vf2d vec = {cosf(direction / 180.f * PI), -sinf(direction / 180.f * PI)};

    position += vec * delta * speed;

    if(turnTimer.getSeconds() > 4) {
        to_direction += float(rand()%220 - 110);
        turnTimer.restart();
    }

    if(position.x > pge->ScreenWidth() || position.x < 0 || position.y > pge->ScreenHeight() || position.y < 0) {
        to_direction = atan2(position.y - pge->ScreenHeight() / 2.f, pge->ScreenWidth() / 2.f - position.x) / float(PI) * 180.f;
    }

    collide = false;
    
    // Collision

    olc::vi2d mpos = pge->GetMousePos();
    float xx = mpos.x, yy = mpos.y;

    float sn = sin(-rotation), cs = cos(-rotation),
            vx_h = cs, vy_h = -sn,
            vx_v = sn, vy_v = cs,
            px = xx - position.x, py = yy - position.y;
    // dot product A
    float dist_h = fabs(px * vx_h + py * vy_h),
    // dot product B
          dist_v = fabs(px * vx_v + py * vy_v);

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

    if(dist_h < frameLocation.size.x*scale.x / 2.f && dist_v < frameLocation.size.y*scale.y / 2.f){
        collide = true;
    }

    // ---------------

    return true;
}


void Car::draw(float delta) {
    Entity::draw(delta); // inherited event
    
    pge->DrawRotatedDecal(position, collisionBoundary, rotation, origin, scale, collide ? olc::RED : olc::WHITE);
    
    testPointA.draw();
    testPointB.draw();
    testPointC.draw();
    testPointD.draw();
    testPointE.draw();
}
