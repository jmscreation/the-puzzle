#include "car_entity.h"

Car::Car(float x, float y): Entity(EntityTypes.car_png_strip),
    direction(0), to_direction(0), speed(0), to_speed(0), turn_speed(8),
    accelleration(20.f), decelleration(80.f), max_speed(96.f),
    destX(0), destY(0), inHurry(false),
    testPointA(0,0, 16,16, olc::BLUE),
    testPointB(0,0, 16,16, olc::GREEN),
    testPointC(0,0, 16,16, olc::YELLOW),
    testPointD(0,0, 16,16, olc::CYAN),
    testPointE(0,0, 16,16, olc::MAGENTA)

    {
    position.x = x;
    position.y = y;
    origin = frameLocation.size / 2.f;
    setFrameRegion("all_cars", rand());
    scale *= 1.5f;
    
    // Collision Boundary Test
    spr = new olc::Sprite(frameLocation.size.x, frameLocation.size.y);
    pge->SetDrawTarget(spr);
    pge->Clear(olc::BLANK);
    pge->DrawRect(0,0,int(frameLocation.size.x - 1), int(frameLocation.size.y - 1));
    pge->SetDrawTarget(nullptr);
    collisionBoundary = new olc::Decal(spr);


    slowdown = -1.f;
}


Car::~Car() {
    delete collisionBoundary;
    delete spr;
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

    if(forwardTimer.getSeconds() > 1){
        to_direction = atan2(position.y - destY, destX - position.x) * 180.f / PI;
        forwardTimer.restart();
    }

    if(turnTimer.getSeconds() > 4) {
        to_direction = 75 + rand() % 30;
        to_speed += float(rand()%120 - 60) / 10.f;

        turnTimer.restart();
    }

    olc::vi2d mpos = pge->GetMousePos();

    if(position.y < -100 || position.x < -100
       || position.x > pge->ScreenWidth() + 100 || position.y > pge->ScreenHeight() + 100
       || (pge->GetMouse(olc::Mouse::RIGHT).bPressed && checkCollision(mpos.x, mpos.y))){
        return false; // destroy if outside or right clicked on
    }
    
    {
        float dist;
        int side=0;
        Car* other;
        if((other = lookAhead(dist, side, 64, 6.f)) != nullptr){
            float dir = atan2(position.y - other->position.y, other->position.x - position.x) * 180.f / PI,
                  raw_angle_toward_diff = fabs(dir - direction),
                  angle_toward_dist = raw_angle_toward_diff > 180.f ? 360.f - raw_angle_toward_diff : raw_angle_toward_diff,
                  sign = fmod(dir - direction + 360.f, 360.f) < 180.f ? -1.f : 1.f, // move away
                  odir = other->direction,
                  raw_ang_diff = fabs(odir - direction),
                  angle_dist = raw_ang_diff > 180.f ? 360.f - raw_ang_diff : raw_ang_diff;
            
            if(slowdown < 0){
                slowdown = to_speed;
            }

            if(dist > 12 && (inHurry || speed > 30)){
                to_direction = direction + sign * angle_toward_dist / 2.f;
            }

            if(stuckTimeout.getSeconds() > 20){
                return false; // destroy if stuck
            } else {
                if(side == 2 && angle_dist > 55){ // take right of way
                    to_speed += delta * 16.f;
                    to_direction += delta * sign * angle_toward_dist / 8.f;
                } else if(side == 1 && angle_dist > 55){ // yield on left
                    to_speed = 0.f;
                } else {
                    if(angle_dist > 3.f || dist < 32.f){
                        to_speed = (angle_dist < 30.f) ?
                                    (dist > 20.f ? other->speed / fmax((angle_dist / dist), 1.001f) : other->speed / 4.f) :
                                    (dist > 20.f ? (angle_dist < 50 ? dist / 4.f : 0.f) : 0.f);
                        if(speed > dist && !side) to_speed /= 2.f;
                    }
                }
            }
            if(stuckTimeout.getSeconds() > 2 && inHurry && side != 0){
                to_speed += delta * 16.f;
            }
        } else {
            stuckTimeout.restart();
            if(slowdown >= 0){
                to_speed = slowdown;
                slowdown = -1.f;
            }
        }
    }

    if(speed > 0.01f){
        stuckTimeout.restart();
    }

    return true;
}


void Car::draw(float delta) {
    Entity::draw(delta); // inherited event
    
    olc::vi2d mpos = pge->GetMousePos();
    
    //pge->DrawRotatedDecal(position, collisionBoundary, rotation, origin, scale, checkCollision(mpos.x, mpos.y) ? olc::RED : olc::WHITE);
    
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

Car* Car::lookAhead(float& foundDistance, int& side, float distance, float step) {
    float dir = (direction) / 180.f * PI,
          vx = cos(dir), vy = -sin(dir), // dir unit vector
          mag_h = (frameLocation.size.y - origin.y) * scale.y,
          mag_v = (frameLocation.size.x - origin.x) * scale.x,
          px = position.x + vx * mag_h, py = position.y + vy * mag_h, // front coord

          _vx = vy, _vy = -vx, // perpendicular vector to corners
          pxTop = px + _vx * mag_v, pyTop = py + _vy * mag_v, // front top corner coord
          pxBtm = px + _vx * -mag_v, pyBtm = py + _vy * -mag_v, // front bottom corner coord
          updir = (direction + 45) / 180.f * PI, // diagonal up
          dwndir = (direction - 45) / 180.f * PI, // diagonal down

          tdir = (to_direction) / 180.f * PI,
          tvx = cos(tdir), tvy = -sin(tdir), // to dir unit vector
          duvx = cos(updir), duvy = -sin(updir), // diagonal up unit vector
          ddvx = cos(dwndir), ddvy = -sin(dwndir); // diagonal down unit vector

    step = std::clamp(step, 0.5f, 1024.f);

    testPointA.x = px;
    testPointA.y = py;

    testPointB.x = px + tvx * distance;
    testPointB.y = py + tvy * distance;

    testPointC.x = px + vx * distance;
    testPointC.y = py + vy * distance;

    testPointD.x = pxTop;
    testPointD.y = pyTop;

    testPointE.x = pxBtm;
    testPointE.y = pyBtm;

    for(float d=1.f; d <= distance; d += step){
        Car *c, *cleft = nullptr, *cright = nullptr, *ctoward = nullptr;
        if(
           (c = Car::checkAllCollision(px + vx * d, py + vy * d, this)) != nullptr ||
           //(c = Car::checkAllCollision(pxTop + vx * d, pyTop + vy * d, this)) != nullptr ||
           //(c = Car::checkAllCollision(pxBtm + vx * d, pyBtm + vy * d, this)) != nullptr ||
           (c = cleft = Car::checkAllCollision(pxTop + duvx * d/2.f, pyTop + duvy * d/2.f, this)) != nullptr ||
           (c = cright = Car::checkAllCollision(pxBtm + ddvx * d/2.f, pyBtm + ddvy * d/2.f, this)) != nullptr ||
           (c = ctoward = Car::checkAllCollision(px + tvx * d, py + tvy * d, this)) != nullptr
        ){
            foundDistance = d;
            side = 0;
            if(cleft != nullptr) side = 1;
            if(cright != nullptr) side = 2;
            if(ctoward != nullptr) side = 3;

            return c;
        }
    }
    return nullptr;
}