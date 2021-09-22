#include "entity.h"

std::vector<olc::AnimationAsset> Entity::animations;
std::vector<Entity*> Entity::entities;

Entity::Entity(size_t asset): olc::Animation(*animations.at(asset)) {
    entities.push_back(this);
}

Entity::~Entity() {}

bool Entity::update(float delta) {

    return true;
}

void Entity::draw(float delta) {
    Draw();
}



/*
    Static Methods
*/

bool Entity::LoadNewEntityAnimation(const std::string& animationName, size_t& assetID) {
    olc::AnimationAsset frame = globalAssetManager.GetAnimation(animationName);
    if(frame == nullptr) return false;

    assetID = animations.size();
    animations.push_back(frame);

    return true;
}

void Entity::UnloadResources() {
    for(Entity* e : entities) delete e;
    entities.clear();
    animations.clear();
}

void Entity::EntityUpdate(float delta) {
    if(!entities.empty()){
        for(int64_t i=entities.size()-1; i >= 0; --i){
            Entity* e = entities[i];
            if(!e->update(delta)){ // check if entity will live
                delete e;
                entities.erase(entities.begin() + i);
            }
        }

        for(int64_t i=entities.size()-1; i >= 0; --i){
            Entity* e = entities[i];
            e->draw(delta);
        }
    }
}