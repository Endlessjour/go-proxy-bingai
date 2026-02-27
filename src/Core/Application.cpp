#include "Core/Application.h"

#include <SDL.h>

namespace Core {

bool Application::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return false;
    }

    if (!renderSystem_.Init("Project Monstoria - Phase 1", 1280, 720)) {
        SDL_Quit();
        return false;
    }

    isRunning_ = true;
    return true;
}

void Application::Run() {
    gameLoop_.Run(
        [this]() {
            ProcessEvents();
            game_.HandleInput();
        },
        [this](double fixedDeltaTime) {
            game_.Update(fixedDeltaTime);
        },
        [this]() {
            game_.Render();
        },
        [this]() {
            return isRunning_;
        }
    );
}

void Application::Shutdown() {
    renderSystem_.Shutdown();
    SDL_Quit();
}

void Application::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning_ = false;
        }
    }
}

} // namespace Core
