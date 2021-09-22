#include "archiveassetmanager.h"

namespace olc {

    std::map<std::string, AssetReference::AssetType> AssetArchiveTypes = {
        {"animation", AssetReference::ANIMATION},
        {"sprite", AssetReference::SPRITE},
        {"sound", AssetReference::SOUND},
        {"font", AssetReference::FONT},
        {"menu", AssetReference::MENU},
        {"file", AssetReference::RAW},
    };


    ArchiveAssetManager::ArchiveAssetManager(): package(new ArchiveFile), AssetManager() {}

    ArchiveAssetManager::~ArchiveAssetManager() {
        delete package;
    }


    AssetReference::AssetType ArchiveAssetManager::GetAssetType(const std::string& type) {
        return AssetArchiveTypes.count(type.c_str()) == 1 ?
                    AssetArchiveTypes[type] : AssetReference::RAW; // check if asset is valid otherwise return raw
    }

    std::string ArchiveAssetManager::GetAssetType(AssetReference::AssetType type) {
        auto t = std::find_if(AssetArchiveTypes.begin(), AssetArchiveTypes.end(),
            [&](auto& ty){ return ty.second == type; });
        if(t != AssetArchiveTypes.end()){
            return t->first; // return asset name
        }
        return "unknown"; // could not find asset
    }

    bool ArchiveAssetManager::LoadArchive(const char* data, size_t length) {
        return package->LoadArchive(data, length);
    }

    std::vector<AssetReference> ArchiveAssetManager::GetResourceList() {
        std::vector<AssetReference> assetList;
        auto list = package->ListFiles();
        for(const auto& f : list) {
            assetList.push_back({
                f.name, // Asset Name
                GetAssetType(f.type) // Asset Type
            });
        }
        return assetList;
    }

    ResourceData ArchiveAssetManager::LoadResource(const std::string& name) {
        ResourceData data;
        ArchiveFile::FileHeader file = package->FindFile(name);

        data.data = file.buffer;
        data.length = file.size;
        data.type = GetAssetType(file.type);

        return data;
    }

}