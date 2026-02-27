#include "Render/RenderSystem.h"

namespace Render {

bool RenderSystem::Init(const char* title, int width, int height) {
    window_ = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (window_ == nullptr) {
        return false;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_ == nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        return false;
    }

    return true;
}

void RenderSystem::BeginFrame() {
    SDL_SetRenderDrawColor(renderer_, 20, 20, 24, 255);
    SDL_RenderClear(renderer_);
}

void RenderSystem::EndFrame() {
    SDL_RenderPresent(renderer_);
}

void RenderSystem::Shutdown() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
}

} // namespace Render
