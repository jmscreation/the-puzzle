#include "textelement.h"


namespace olc {
    
    TextElement::TextElement(olc::FontAsset fnt, const std::string& string):
            font(fnt), textData(string), texture(nullptr), sprite(nullptr), color(olc::WHITE),
            size({0, 0}), hTextAlign(Align::NONE), vTextAlign(Align::NONE) {
        UpdateTexture();
    }

    TextElement::TextElement(olc::FontAsset fnt):
            font(fnt), textData(""), texture(nullptr), sprite(nullptr), color(olc::WHITE),
            size({0, 0}), hTextAlign(Align::NONE), vTextAlign(Align::NONE) {
        UpdateTexture();
    }

    TextElement::~TextElement() {
        delete texture;
        delete sprite;
    }

    void TextElement::UpdateTexture() {

        if(texture == nullptr || sprite == nullptr){
            size = font->GetStringBounds(textData).size;

            size = size.max(olc::vi2d({1024,1024}));

            sprite = new olc::Sprite(size.x, size.y);
            texture = new olc::Decal(sprite);
        }

        if(font->RenderStringToSprite(textData, color, sprite) == nullptr) return;

        size = font->GetRenderedStringBounds().size;

        switch(hTextAlign){ // update x origin based on auto text alignment
            case Align::NONE: break;
            case Align::LEFT: origin.x = 0; break;
            case Align::RIGHT: origin.x = size.x; break;
        }

        switch(vTextAlign){ // update y origin based on auto text alignment
            case Align::NONE: break;
            case Align::TOP: origin.y = 0; break;
            case Align::BOTTOM: origin.y = size.y; break;
        }

        texture->Update();
    }

    void TextElement::UpdateString(const std::string& string) {
        textData = string;

        UpdateTexture();
    }

    void TextElement::Draw(View* view) {
        if(view != nullptr) {
            view->DrawRotatedDecal(position, texture, rotation, origin, scale, blend);
        } else {
            pge->DrawRotatedDecal(position, texture, rotation, origin, scale, blend);
        }
    }
}