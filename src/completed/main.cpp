#include "main.h"

namespace fs = std::filesystem;

olc::ArchiveAssetManager* globalAssetManager = nullptr;
std::vector<std::string> programArguments;
EntityAnimation EntityTypes;


bool initAssetManager() {
    std::cout << "Init Asset Manager...\n";
    assert(globalAssetManager == nullptr); // cannot load more than once

    globalAssetManager = new olc::ArchiveAssetManager(); // instantiate new asset manager

    globalAssetManager->LoadArchive(puzzle_asset_archive, sizeof(puzzle_asset_archive));

    std::cout << "Check Resources...\n";
    auto list = globalAssetManager->GetResourceList();

    for(auto& r : list){
        std::cout << "Resource: " << r.name << "\n"
                  << "  Type: " << globalAssetManager->GetAssetType(r.type) << "\n";
    }

    return true;
}

void freeAssetManager() {
    delete globalAssetManager;
    globalAssetManager = nullptr;
}

bool launchGame() {
    Window win; // spawn game window and run game
    olc::rcode rval = win.Construct(500, 500, 1, 1);
    if(rval == olc::OK) {
        rval = win.Start();
    }
    if(rval == olc::OK) return true;

    std::cout << "program terminated with error: " << rval << "\n";
    return false;
}

int main(int argc, char** argv){
    srand((uint32_t)std::chrono::system_clock::now().time_since_epoch().count());
    for(int i=1; i < argc; ++i){
        programArguments.push_back(argv[i]); // create argument list
    }

    #ifdef NO_RELEASE
    //freedialog::MessageDialog msg("This project is currently under development. Please check back later!", "Unavailable", MB_ICONERROR);
    return 0;
    #endif

    if(!initAssetManager()){
        std::cout << "something went wrong when initializing asset manager\n";
        return 1;
    }

    if(!launchGame()){
        std::cout << "something went wrong when launching game\n";
        return 1;
    }


    freeAssetManager();

    std::cout << "program terminated\n";

    return 0;
}