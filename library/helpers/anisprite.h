#ifndef __ANISPRITE__
#define __ANISPRITE__

#include "olcPixelGameEngine.h"
#include "assettypes.h"
#include "element.h"

namespace olc {
    
    // Data struct for storing frame information

    class Animation : public Element {
    protected:
        uint32_t curFrame; // current image selected to display - must be between 0 and frame count
        std::string curRegion; // current image region of images - string name of region
        uint32_t curRegionFrame; // current image selected relative to the region - this number is mod the selected region size
        olc::vf2d curPosition; // used for current frame offset

        void translateFrame();
        void setRawFrame(uint32_t frame);
        inline uint32_t getRawFrame() const { return curFrame; }

    public:

        Frame frameLocation;

        void setFrame(uint32_t frame);
        inline uint32_t getFrame() const { return curRegionFrame; }

        void setFrameRegion(uint32_t freg, uint32_t relativeFrame=0);
        void setFrameRegion(const std::string& freg, uint32_t relativeFrame=0);
        inline std::string getFrameRegion() const { return curRegion; }
        inline uint32_t getFrameRegionCount() const { return frameLocation.region.size(); }

        Animation(const Frame& animationData);
        virtual ~Animation();

        virtual void Draw(View* view=nullptr);
    };

}



#endif // __ANISPRITE__