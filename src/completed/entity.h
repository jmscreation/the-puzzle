#pragma once

#include "external_defines.h"
#include "olcPixelGameEngine.h"
#include "anisprite.h"

#include <vector>

class Entity : public olc::Animation {
public:
    static std::vector<Entity*> entities; // all game entities stored here
private:
    static std::vector<olc::AnimationAsset> animations; // all game entity animation assets stored here

protected:
    virtual ~Entity(); // do not manually delete entity - must return false from update loop

public:
    Entity(size_t asset);

    virtual bool update(float delta); // main update loop for each entity - return false to destroy entity
    virtual void draw(float delta); // main draw loop for each entity

    static bool LoadNewEntityAnimation(const std::string& animationName, size_t& assetID);
    static void UnloadResources();

    static void EntityUpdate(float delta);
};