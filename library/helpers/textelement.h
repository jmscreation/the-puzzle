#ifndef __TEXTELEMENT__
#define __TEXTELEMENT__

#include "element.h"
#include "assettypes.h"

namespace olc {
    
    class TextElement : public Element {
        std::string textData;
        olc::FontAsset font;
        olc::Decal* texture;
        olc::Sprite* sprite;
        olc::Pixel color;
        olc::vi2d size;

        void UpdateTexture();
    public:
        enum Align {
            CENTER, LEFT, RIGHT, TOP, BOTTOM, NONE
        } vTextAlign, hTextAlign;

        TextElement(olc::FontAsset fnt, const std::string& string);
        TextElement(olc::FontAsset fnt);
        virtual ~TextElement();

        inline olc::vi2d GetSize() { return size; }

        virtual void UpdateString(const std::string& string);
        virtual void Draw(View* view=nullptr);

    };
}


#endif //__TEXTELEMENT__