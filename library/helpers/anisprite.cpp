#include "anisprite.h"

namespace olc {

    // Construct
    Animation::Animation(const Frame& animationData):
        curFrame(0), curRegion("default"), curRegionFrame(0), curPosition({0.0f,0.0f}), frameLocation(animationData) {}

    // Destruct
    Animation::~Animation() {}


    // Update the relative frame to the frame region
    void Animation::setFrame(uint32_t frame) {
        curRegionFrame = frame;
        translateFrame();
    }

    // Update the frame region by integer
    void Animation::setFrameRegion(uint32_t freg, uint32_t relativeFrame) {
        auto& list = frameLocation.region;
        if(freg >= list.size()) return;

        auto pos = list.begin();
        for(uint32_t i=0; i++ < freg; pos++);
        std::string name = pos->first;

        setFrameRegion(name, relativeFrame);
    }

    // Update the frame region
    void Animation::setFrameRegion(const std::string& freg, uint32_t relativeFrame) {
        auto& list = frameLocation.region;
        if(std::find_if(list.begin(), list.end(), [&](auto& r){ return r.first == freg; }) == list.end()) return; // animation region doesn't exist

        curRegion = freg;
        auto& region = frameLocation.region[curRegion];
        setFrame(relativeFrame);
    }

    // Translate relative frame to actual frame
    void Animation::translateFrame() {
        auto& region = frameLocation.region[curRegion];

        curRegionFrame = region.length == 0 ? 0 // cannot modulate by 0
            : curRegionFrame % region.length; // modulate the new region frame to the size of the region

        setRawFrame(region.start + curRegionFrame);
    }

    // Update the raw frame offset in the main frame list
    void Animation::setRawFrame(uint32_t frame) {
        curFrame = frame; // for reading purpose

        uint32_t xframe = frame % frameLocation.rowsize, // x frame number
                 yframe = frame / frameLocation.rowsize; // y frame number
        
        // calculate:     offset  +  image offset   +  image spread
        curPosition.x = frameLocation.offset.x + xframe * frameLocation.size.x + xframe * frameLocation.spread.x;
        curPosition.y = frameLocation.offset.y + yframe * frameLocation.size.y + yframe * frameLocation.spread.y;
    }

    // Draw
    void Animation::Draw(View* view) {
        if(frameLocation.texture == nullptr) return;
        if(view != nullptr) {
            view->DrawPartialRotatedDecal(position, &frameLocation.texture->texture, rotation, origin, curPosition, frameLocation.size, scale, blend);
        } else {
            pge->DrawPartialRotatedDecal(position, &frameLocation.texture->texture, rotation, origin, curPosition, frameLocation.size, scale, blend);
        }
    }

}