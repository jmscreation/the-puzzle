#include "element.h"


namespace olc {

    Element::Element():
        position({0.0f, 0.0f}), scale({1.0f, 1.0f}),
        origin({0.0f, 0.0f}), rotation(0.0f), blend(olc::WHITE) {}

    Element::~Element() {}
    

}
