#include "map.h"

DataManipulator::DataManipulator(const char* data, size_t length): error(false), pos(0), length(length), data(data) {}

Map::Map(): _points(mapPoints)  {

}


Map::~Map() {

}


bool Map::LoadMap(const std::string& mapname){

    olc::RawAsset map;
    map = globalAssetManager->GetRaw(mapname);

    if(!map.length || map.type != olc::AssetReference::RAW) return false;

    mapPoints.clear();
    Road::clearRoads();
    Car::clearCars();

    DataManipulator loader(map.data, map.length);
    MapHeader header;
    loader.readData(header);

    for(size_t i=0; i < header.pointCount; ++i){
        uint32_t x, y;
        uint8_t connections;
        mapPoints.emplace_back(MapPoint({})); // add empty MapPoint
        MapPoint& point = mapPoints.back(); // get new MapPoint reference
        
        loader.readData(x);
        loader.readData(y);
        loader.readData(connections);
        point.pos.x = x;
        point.pos.y = y;
        point.connections = std::min(connections, (uint8_t)64);

        for(uint8_t c=0;c < point.connections; ++c){
            uint16_t ind;
            loader.readData(ind);
            point.connect[c] = ind;
        }

        point.dbgpoint = new DebugPoint(x, y);
    }

    for(size_t i=0; i < header.roadCount; ++i) {
        uint16_t x, y;
        uint8_t type, dir;

        loader.readData(x);
        loader.readData(y);
        loader.readData(type);
        loader.readData(dir);

        new Road(x, y, dir, type);
    }

    return true;
}