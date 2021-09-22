#ifndef __ELEMENT__
#define __ELEMENT__

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

typedef olc::TransformedView View;

namespace olc {

    class Element : public olc::PGEX {
    protected:
        // static olc::PixelGameEngine* pge;

    public:
        // static inline void SetPGE(olc::PixelGameEngine* pge) { Element::pge = pge; } // update pge pointer

        Element();
        virtual ~Element();

        virtual void Draw(View* view=nullptr) = 0;

    public:
        olc::vf2d position;
        olc::vf2d scale;
        olc::vf2d origin;
        float rotation; // radians
        olc::Pixel blend;

    };

}


#endif //__ELEMENT__