#include "map.h"

DataManipulator::DataManipulator(): readonly(false), error(false), pos(0), length(0), wdata( std::make_unique<std::stringstream>() ) {}
DataManipulator::DataManipulator(const char* data, size_t length):
                                    readonly(true), error(false), pos(0), length(length), rdata(data) {}

Map::Map(): _points(mapPoints)  {}

Map::~Map() {}


size_t Map::AddMapPoint(float x, float y) {
    mapPoints.emplace_back(MapPoint({})); // add empty MapPoint
    MapPoint& point = mapPoints.back(); // get new MapPoint reference

    std::cout << "add point: " << x << " " << y << "\n";

    point.connections = 0;
    point.pos.x = x;
    point.pos.y = y;

    point.dbgpoint = new DebugPoint(x, y);

    return mapPoints.size() - 1; // new index
}


bool Map::LoadMapFile(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if(!file.is_open()) return false;

    std::stringstream buf;
    buf << file.rdbuf();
    file.close();
    if(!buf.good()) return false;

    const std::string& data = buf.str();

    return LoadMapBuffer(data.data(), data.length());
}

bool Map::LoadMap(const std::string& mapname) {
    olc::RawAsset map;
    map = globalAssetManager->GetRaw(mapname);

    if(!map.length || map.type != olc::AssetReference::RAW) return false;

    return LoadMapBuffer(map.data, map.length);
}

bool Map::LoadMapBuffer(const char* data, size_t length) {

    DataManipulator loader(data, length);

    MapHeader header;
    loader.readData(header);

    std::cout << "header id:" << header.id << "\n"
              << "version: " << header.version << "\n"
              << "points: " << header.pointCount << "\n"
              << "roads: " << header.roadCount << "\n";

    if(header.id != MAP_HEADER_ID) return false; // invalid header id

    mapPoints.clear();
    Road::clearRoads();
    Car::clearCars();

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

        std::cout << "\tload road: x=" << x << " y=" << y << " type=" << (int)type << " dir=" << (int)dir << "\n";

        new Road(x, y, dir, type);
    }

    return true;
}

bool Map::SaveMap(const std::string& path) {

    DataManipulator writer;

    MapHeader header;

    header.id = MAP_HEADER_ID;
    header.version = MAP_HEADER_VERSION;
    header.pointCount = mapPoints.size();
    header.roadCount = std::accumulate(Entity::entities.begin(), Entity::entities.end(), 0,
                                    [](int i, Entity* e){ return i + !!(dynamic_cast<Road*>(e) != nullptr && e->blend.a >= 250); });

    writer.writeData(header);

    for(size_t i=0; i < header.pointCount; ++i){
        const MapPoint& point = mapPoints[i]; // get existing MapPoint reference
        
        uint32_t x = std::round(point.pos.x),
                 y = std::round(point.pos.y);
        
        uint8_t connections = point.connections;

        writer.writeData(x);
        writer.writeData(y);
        writer.writeData(connections);

        for(uint8_t c=0;c < connections; ++c){
            uint16_t ind;
            ind = point.connect[c];
            writer.writeData(ind);
        }
    }

    for(const Entity* e : Entity::entities){
        const Road* road = dynamic_cast<const Road*>(e); // cast to road
        if(road == nullptr || road->blend.a < 250) continue; // not a valid road - skip

        uint16_t x = std::round((road->position.x + road->origin.x*road->scale.x - road->width) / road->width),
                 y = std::round((road->position.y + road->origin.y*road->scale.y - road->height) / road->height);
        uint8_t type = road->getFrame(), dir = (uint8_t) std::floor( road->rotation / (PI / 2.f) );

        writer.writeData(x);
        writer.writeData(y);
        writer.writeData(type);
        writer.writeData(dir);
    }

    writer.writeData("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"); // append 16 null bytes

    std::stringstream mapbuffer;
    if(!writer.exportData(mapbuffer)) return false;

    std::ofstream file(path, std::ios::out | std::ios::binary);
    if(!file.is_open()){
        std::cout << "write file error\n";
        return false;
    }

    bool result = (file << mapbuffer.rdbuf()).good();

    file.close();
    return result;
}