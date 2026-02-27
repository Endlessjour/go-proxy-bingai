#include "Core/GameLoop.h"

#include <SDL.h>

namespace Core {

void GameLoop::Run(
    const TickCallback& input,
    const UpdateCallback& update,
    const TickCallback& render,
    const std::function<bool()>& running
) {
    while (running()) {
        const std::uint32_t frameStart = SDL_GetTicks();

        input();
        update(kFixedDeltaTime);
        render();

        const std::uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < kFrameMs) {
            SDL_Delay(kFrameMs - frameTime);
        }
    }
}

} // namespace Core
