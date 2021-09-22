#ifndef __ASSET_MANAGER__
#define __ASSET_MANAGER__

#include "assettypes.h"
#include "olcPGEX_TTF.h"

#include "memloader.h"
#include "anisprite.h" // olc::Animation
#include "jsonloader.h" // load from a json file format

#include <sstream>
#include <variant>
#include <vector>


namespace olc {

    class AssetManager {
    protected:
        typedef std::variant<std::monostate, SpriteAsset, SoundAsset, AnimationAsset, FontAsset> GenericAsset;
        typedef std::variant<std::monostate, TextureAsset> SecondaryAsset;

        memloader::MemoryImageLoader* imageLoader;
    public:
        AssetManager();
        virtual ~AssetManager();
        
        virtual void CleanupMemory();

        virtual std::vector<AssetReference> GetResourceList() = 0;

        virtual FontAsset GetFont(const std::string& name);
        virtual AnimationAsset GetAnimation(const std::string& name);
        virtual TextureAsset GetTexture(const std::string& name);
        virtual SpriteAsset GetSprite(const std::string& name);
        virtual SoundAsset GetSound(const std::string& name);
        virtual RawAsset GetRaw(const std::string& name);


    protected:
        std::map<std::string, GenericAsset> assets;
        std::map<std::string, SecondaryAsset> otherAssets;
        std::map<std::string, RawAsset> rawAssets;

        virtual ResourceData LoadResource(const std::string& name) = 0;

    };

}

#endif // __ASSET_MANAGER__