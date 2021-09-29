#pragma once

#include <iostream>
#include <string>
#include <type_traits>

#include "debug_point.h"
#include "road_entity.h"
#include "car_entity.h"
#include "external_defines.h"
#include "olcPixelGameEngine.h"

class DataManipulator {
    bool error;
    size_t pos, length;
    const char* data;

public:

    template<class T>
    bool readData(T& rval) {
        int sz = sizeof(rval);

        if(pos + sz - 1 >= length){
            error = true;
            return false;
        }

        memcpy(reinterpret_cast<char*>(&rval), data + pos, sz);
        
        pos += sz;
        return true;
    };

    DataManipulator(const char* data, size_t length);

};

struct MapPoint {
    olc::vf2d pos;
    uint8_t connections;
    uint16_t connect[64];

    DebugPoint* dbgpoint;
};


class Map {
    std::vector<MapPoint> mapPoints;

    const std::vector<MapPoint>& _points;

    struct MapHeader {
        size_t id, version, pointCount, roadCount;
    };

public:
    Map();
    virtual ~Map();

    inline const std::vector<MapPoint>& GetMapPoints() { return _points; }

    bool LoadMap(const std::string& mapname);

};