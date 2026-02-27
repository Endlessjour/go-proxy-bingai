#include "Entity/Player.h"

#include "World/Map.h"

#include <algorithm>
#include <cmath>

namespace Entity {

void Player::SetSpawn(int tileX, int tileY) {
    tileX_ = tileX;
    tileY_ = tileY;
    offsetX_ = 0.0f;
    offsetY_ = 0.0f;
}

void Player::TryMove(int dirX, int dirY, const World::Map& map) {
    if (IsMoving()) {
        return;
    }

    const int targetX = tileX_ + dirX;
    const int targetY = tileY_ + dirY;
    if (map.IsBlocked(targetX, targetY)) {
        return;
    }

    tileX_ = targetX;
    tileY_ = targetY;

    offsetX_ = static_cast<float>(-dirX * map.GetTileWidth());
    offsetY_ = static_cast<float>(-dirY * map.GetTileHeight());
}

void Player::Update(double deltaSeconds, int tileWidth, int tileHeight) {
    const float stepX = static_cast<float>(kMoveSpeedTilesPerSecond * static_cast<float>(deltaSeconds) * tileWidth);
    const float stepY = static_cast<float>(kMoveSpeedTilesPerSecond * static_cast<float>(deltaSeconds) * tileHeight);

    if (offsetX_ < 0.0f) {
        offsetX_ = std::min(0.0f, offsetX_ + stepX);
    } else if (offsetX_ > 0.0f) {
        offsetX_ = std::max(0.0f, offsetX_ - stepX);
    }

    if (offsetY_ < 0.0f) {
        offsetY_ = std::min(0.0f, offsetY_ + stepY);
    } else if (offsetY_ > 0.0f) {
        offsetY_ = std::max(0.0f, offsetY_ - stepY);
    }
}

void Player::Render(SDL_Renderer* renderer, int tileWidth, int tileHeight) const {
    if (renderer == nullptr) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 220, 64, 255);
    SDL_Rect playerRect{
        static_cast<int>(tileX_ * tileWidth + offsetX_),
        static_cast<int>(tileY_ * tileHeight + offsetY_),
        tileWidth,
        tileHeight
    };
    SDL_RenderFillRect(renderer, &playerRect);
}

bool Player::IsMoving() const {
    return std::fabs(offsetX_) > 0.01f || std::fabs(offsetY_) > 0.01f;
}

} // namespace Entity
