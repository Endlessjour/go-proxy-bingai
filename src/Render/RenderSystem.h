#pragma once

#include <SDL.h>

namespace Render {

class RenderSystem {
public:
    bool Init(const char* title, int width, int height);
    void BeginFrame();
    void EndFrame();
    void Shutdown();

    SDL_Renderer* GetRenderer() const { return renderer_; }
    SDL_Window* GetWindow() const { return window_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
};

} // namespace Render
