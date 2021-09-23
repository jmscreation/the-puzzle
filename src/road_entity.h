#pragma once

#include "entity.h"
#include "external_defines.h"

class Road : public Entity {
protected:
    virtual ~Road();

public:
    static float width, height;

    

    Road(int cellx, int celly, char dir, int type);

    bool update(float delta) override;
    void updatePlacement(int cellx, int celly, char dir, int type);

    // void draw(float delta) override;

};