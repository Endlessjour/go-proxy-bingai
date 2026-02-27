#pragma once

#include <SDL.h>

namespace World {
class Map;
}

namespace Entity {

class Player {
public:
    void SetSpawn(int tileX, int tileY);

    void TryMove(int dirX, int dirY, const World::Map& map);
    void Update(double deltaSeconds, int tileWidth, int tileHeight);
    void Render(SDL_Renderer* renderer, int tileWidth, int tileHeight) const;

    bool IsMoving() const;
    int GetTileX() const { return tileX_; }
    int GetTileY() const { return tileY_; }

private:
    int tileX_ = 1;
    int tileY_ = 1;

    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;

    static constexpr float kMoveSpeedTilesPerSecond = 6.0f;
};

} // namespace Entity
