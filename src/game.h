#pragma once

#include "external_defines.h"
#include "olcPixelGameEngine.h"
#include "archiveassetmanager.h"
#include "clock.h"
#include "entity.h"
#include "game_controller.h"
#include "debug_point.h"

#include <vector>


class Window : public olc::PixelGameEngine {
    olc::ArchiveAssetManager& manager;
    Clock runtime;
    Controller* controller;

public:
    Window();

    virtual ~Window();

    bool OnUserCreate() override;
    bool OnUserUpdate(float delta) override;
    bool OnUserDestroy() override;
    
};