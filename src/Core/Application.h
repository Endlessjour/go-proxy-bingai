#pragma once

#include "Core/GameLoop.h"
#include "Game/Game.h"
#include "Render/RenderSystem.h"

namespace Core {

class Application {
public:
    bool Init();
    void Run();
    void Shutdown();

private:
    void ProcessEvents();

    bool isRunning_ = false;
    Render::RenderSystem renderSystem_;
    Game::Game game_{renderSystem_};
    GameLoop gameLoop_;
};

} // namespace Core
