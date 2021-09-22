#ifndef __ASSET_TYPES__
#define __ASSET_TYPES__

#include "olcPixelGameEngine.h"
#include "olcPGEX_TTF.h"
#include "audio.h"
#include <memory>
#include <string>

/*
    Important Asset Types And Structs Defined Here
*/

namespace olc {
    
    // The order in which the following are declared is important

    struct AssetReference {
        std::string name;
        enum AssetType {
            SPRITE, SOUND, RAW, ANIMATION, FONT, MENU
        } type;
    };

    struct ResourceData {
        const char* data;
        size_t length;
        AssetReference::AssetType type;
    };

    typedef ResourceData RawAsset;

    typedef std::shared_ptr<olc::Sprite> SpriteAsset;

    struct ResourceTexture {
        SpriteAsset sprite;
        olc::Decal texture;
    };
    typedef std::shared_ptr<ResourceTexture> TextureAsset;


    struct Sequence {
        uint32_t start, length;
    };

    struct Frame {
        olc::vf2d offset; // frame source start position
        olc::vf2d size; // image width / height
        olc::vf2d spread; // pixel offset per image

        uint32_t count; // image total count
        uint32_t rowsize; // image count per row

        std::map<std::string, Sequence> region; // animation region

        TextureAsset texture; // actual texture image used
    };

    typedef std::shared_ptr<Frame> AnimationAsset;
    typedef std::shared_ptr<olc::SoundBuffer> SoundAsset;
    typedef std::shared_ptr<olc::Font> FontAsset;

}

#endif // __ASSET_TYPES__