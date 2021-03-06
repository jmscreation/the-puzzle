#include "entity.h"

std::vector<olc::AnimationAsset> Entity::animations;
std::list<Entity*> Entity::entities;

Entity::Entity(size_t asset): olc::Animation(*animations.at(asset)), depth(0.f), destroy(false) {
    entities.insert(entities.begin(), this);
}

Entity::~Entity() {}

bool Entity::update(float delta) {

    return true;
}

void Entity::draw(float delta) {
    Draw();
}

void Entity::setDepth(float depth) {
    assert(depth >= 0.f); // cannot set a depth lower than 0

    this->depth = depth;
    auto me = entities.begin();
    for(;me != entities.end(); ++me){
        if(*me == this) break;
    }
    if(me == entities.end()){
        std::cout << "me is missing\n";
        entities.push_back(this); // last element
        --me; // me is now the last element - it's okay because this will be moved in next phase
    }
    assert(me != entities.end()); // me should never be the end of the list

    auto i = entities.begin();
    for(; i != entities.end(); ++i){
        Entity* e = *i;
        if(e->depth < depth || i == me) continue;
        break;
    }
    entities.splice(i, entities, me); // move right in front of the member
}


/*
    Static Methods
*/

bool Entity::LoadNewEntityAnimation(const std::string& animationName, size_t& assetID) {
    olc::AnimationAsset frame = globalAssetManager->GetAnimation(animationName);
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
    /*
    static Clock dbgTimer;
    bool dbg = false;
    if(dbgTimer.getSeconds() > 1){
        dbg = true;
        dbgTimer.restart();
    }
    */
    if(!entities.empty()){
        //if(dbg) std::cout << "debug: [";
        for(auto i=entities.rbegin(); i != entities.rend(); ++i){
            Entity* e = *i;
            if(e->destroy || !e->update(delta)){ // check if entity will live
                delete e;
                entities.erase(std::next(i).base());
                --i;
            }
        }
        for(auto i=entities.rbegin(); i != entities.rend(); ++i){
            Entity* e = *i;
            e->draw(delta);
            //if(dbg) std::cout << e->getDepth() << ",";
        }

        //if(dbg) std::cout << "]\n";
    }
}
