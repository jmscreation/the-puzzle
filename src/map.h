#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <numeric>
#include <type_traits>

#include "debug_point.h"
#include "road_entity.h"
#include "car_entity.h"
#include "external_defines.h"
#include "olcPixelGameEngine.h"

#define MAP_HEADER_ID       849574238543247389
#define MAP_HEADER_VERSION  1000


class DataManipulator {

    bool readonly, error;
    size_t pos, length;

    union {
        const char* rdata;
        std::unique_ptr<std::stringstream> wdata;
    };

public:

    template<class T>
    bool readData(T& rval) {
        if(!readonly) return false;

        int sz = sizeof(rval);

        if(pos + sz - 1 >= length){
            error = true;
            return false;
        }

        memcpy(reinterpret_cast<char*>(&rval), rdata + pos, sz);
        
        pos += sz;
        return true;
    };

    template<class T>
    bool writeData(const T& rval) {
        if(readonly) return false;
        int sz = sizeof(rval);

        return wdata->write(reinterpret_cast<const char*>(&rval), sz).good();
    };

    inline bool exportData(std::stringstream& stream) { // append data to stream
        if(readonly) return false;

        return (stream << wdata->rdbuf()).good();
    }

    DataManipulator(); // writer
    DataManipulator(const char* data, size_t length); // reader

    virtual ~DataManipulator() {}
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

    size_t AddMapPoint(float x, float y);

    bool LoadMapBuffer(const char* data, size_t length);

    bool LoadMap(const std::string& mapname);
    bool LoadMapFile(const std::string& path);
    bool SaveMap(const std::string& path);

};