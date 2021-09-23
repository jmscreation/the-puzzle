#pragma once

#include "entity.h"
#include "external_defines.h"

class Road : public Entity {
public:
    static float width, height;

    Road(int cellx, int celly, char dir, int type);
    virtual ~Road();

    bool update(float delta) override;
    // void draw(float delta) override;

};