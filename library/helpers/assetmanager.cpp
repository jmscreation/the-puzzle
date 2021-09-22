#include "assetmanager.h"

namespace olc {

    AssetManager::AssetManager() {
        olc::Font::init();
        imageLoader = new memloader::MemoryImageLoader();
        json::displayErrors = true;
    }

    AssetManager::~AssetManager() {
        CleanupMemory();
        delete imageLoader;
    }

    void AssetManager::CleanupMemory() {
        otherAssets.clear();
        assets.clear();
        rawAssets.clear();
    }

    // Self Instantiated Asset Loaders - Secondary Asset Database Managed


    TextureAsset AssetManager::GetTexture(const std::string& name) {

        TextureAsset texture;
        if(otherAssets.count(name) == 1) {
            auto ptr = std::get_if<TextureAsset>(&otherAssets[name]);
            if(ptr == nullptr) return nullptr;

            texture = *ptr;
        } else {
            SpriteAsset sprite = GetSprite(name);
            if(sprite == nullptr) return nullptr;

            texture.reset(new ResourceTexture({sprite, olc::Decal(sprite.get())}));
            otherAssets.insert({name, SecondaryAsset(texture) });
        }

        return texture;
    }

    // Load From Archive

    FontAsset AssetManager::GetFont(const std::string& name) {
        FontAsset font;
        if(assets.count(name) == 1) {
            auto ptr = std::get_if<FontAsset>(&assets[name]);
            if(ptr == nullptr) return nullptr;

            font = *ptr;
        } else {
            ResourceData raw = GetRaw(name);
            if(raw.type != AssetReference::FONT) return nullptr;
            if(raw.length == 0) return nullptr;
            
            font.reset(new olc::Font((FT_Byte*) raw.data, raw.length, 32));
            assets.insert({name, GenericAsset(font) });
        }
        return font;
    }

    AnimationAsset AssetManager::GetAnimation(const std::string& name) {

        AnimationAsset animation;
        if(assets.count(name) == 1) {
            auto ptr = std::get_if<AnimationAsset>(&assets[name]);
            if(ptr == nullptr) return nullptr;

            animation = *ptr;
        } else {
            ResourceData raw = GetRaw(name);
            if(raw.type != AssetReference::ANIMATION) return nullptr;
            std::string texName;

            if(!json::parseData(raw.data, raw.length)){ // invalid json
                std::cout << "failed to parse json data for animation \"" << name << "\"\n";
                return nullptr;
            }

            json::loadProperty("image", texName);                   // texture name to load

            TextureAsset tex = GetTexture(texName);
            if(tex == nullptr){
                std::cout << "animation failed to load image \"" << texName << "\"\n";
                return nullptr; // animation image failed to load
            }

            animation.reset(new olc::Frame());
            animation->texture = tex; // update animation texture
            animation->spread = {0.0f, 0.0f};
            animation->offset = {0.0f, 0.0f};
            animation->count = 1;
            animation->rowsize = 1;
            animation->size = {float(tex->sprite->width), float(tex->sprite->height)};

            json::Object offset, spread, sequences;
            json::loadProperty("offset", offset);                   // frame initial offset
            json::loadProperty("spread", spread);                   // frame spread distance
            json::loadProperty("sequences", sequences);             // frame regions
            json::loadProperty(offset, "x", animation->offset.x);   // frame initial x offset
            json::loadProperty(offset, "y", animation->offset.y);   // frame initial y offset
            json::loadProperty(spread, "x", animation->spread.x);   // per frame x distance
            json::loadProperty(spread, "y", animation->spread.y);   // per frame y distance
            json::loadProperty("width", animation->size.x);         // frame width
            json::loadProperty("height", animation->size.y);        // frame height
            json::loadProperty("count", animation->count);          // total images
            json::loadProperty("rowsize", animation->rowsize);      // total images per row
            
            animation->region.insert_or_assign("default", (Sequence) {0, animation->count}); // generate a default animation sequence

            if(sequences.IsObject()){
                for(rapidjson::Value::ConstMemberIterator it = sequences.MemberBegin(); it != sequences.MemberEnd(); ++it){
                    if(!it->name.IsString()){
                        std::cout << "skipping non-string key\n";
                        continue;
                    }
                    
                    Sequence data {0, 0};
                    json::loadProperty(it->value, "start", data.start);
                    json::loadProperty(it->value, "length", data.length);
                    
                    animation->region.insert_or_assign(it->name.GetString(), data);
                }
            }

            assets.insert({name, GenericAsset(animation) });
        }
        return animation;
    }

    SpriteAsset AssetManager::GetSprite(const std::string& name) {

        SpriteAsset sprite;
        if(assets.count(name) == 1) {
            auto ptr = std::get_if<SpriteAsset>(&assets[name]);
            if(ptr == nullptr) return nullptr;

            sprite = *ptr;
        } else {
            ResourceData raw = GetRaw(name);
            if(raw.type != AssetReference::SPRITE) return nullptr;

            sprite.reset(new olc::Sprite());
            if(raw.length != 0){
                if(!imageLoader->LoadImageResource(sprite.get(), raw.data, raw.length))
                    return nullptr;
            }
            assets.insert({name, GenericAsset(sprite) });
        }
        return sprite;
    }

    SoundAsset AssetManager::GetSound(const std::string& name) {

        SoundAsset sound;
        if(assets.count(name) == 1) {
            auto ptr = std::get_if<SoundAsset>(&assets[name]);
            if(ptr == nullptr) return nullptr;

            sound = *ptr;
        } else {
            ResourceData raw = GetRaw(name);
            if(raw.type != AssetReference::SOUND) return nullptr;

            sound.reset(new olc::SoundBuffer()); // load port audio sound instance

            if(raw.length != 0){
                if(!sound->loadOGGFromMemory(raw.data, raw.length))
                    return nullptr;
            }
            assets.insert({name, GenericAsset(sound)});
        }
        return sound;
    }

    // Load Raw Buffer Data For Asset

    RawAsset AssetManager::GetRaw(const std::string& name) {
        ResourceData rval;
        if(rawAssets.count(name) == 1) {
            rval = rawAssets[name];
        } else {
            rval = LoadResource(name);
            rawAssets.insert({name, rval});
        }
        return rval;
    }

}