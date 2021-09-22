#pragma once

#define PI 3.14159

#include "archiveassetmanager.h"
#include <vector>
#include <string>

struct EntityAnimation {
    size_t test_animation,
           car_png_strip;
};

extern EntityAnimation EntityTypes;

extern std::vector<std::string> programArguments;
extern olc::ArchiveAssetManager globalAssetManager; // global asset manager for loading resources