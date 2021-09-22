#ifndef __ARCHIVE_ASSET_MANAGER__
#define __ARCHIVE_ASSET_MANAGER__

#include "assetmanager.h"
#include "archivefile.h"

namespace olc {

    extern std::map<std::string, AssetReference::AssetType> AssetArchiveTypes;

    class ArchiveAssetManager : public AssetManager {
    protected:
        
        ArchiveFile* package;
    public:

        ArchiveAssetManager();
        virtual ~ArchiveAssetManager();

        static AssetReference::AssetType GetAssetType(const std::string& type);
        static std::string GetAssetType(AssetReference::AssetType type);

        virtual bool LoadArchive(const char* data, size_t length);
        virtual std::vector<AssetReference> GetResourceList() override;

    protected:

        virtual ResourceData LoadResource(const std::string& name) override;
        
    };

}


#endif // __ARCHIVE_ASSET_MANAGER__