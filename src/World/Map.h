#pragma once

#include "World/TileLayer.h"

#include <SDL.h>

#include <optional>
#include <string>
#include <vector>

namespace World {

struct MapEvent {
    int tileX = 0;
    int tileY = 0;
    std::string script;
};

class Map {
public:
    bool LoadFromTmx(const std::string& tmxPath);
    void Render(SDL_Renderer* renderer) const;

    bool IsBlocked(int tileX, int tileY) const;
    bool IsEncounterTile(int tileX, int tileY) const;
    std::optional<MapEvent> GetEventAt(int tileX, int tileY) const;

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    int GetTileWidth() const { return tileWidth_; }
    int GetTileHeight() const { return tileHeight_; }

private:
    static std::vector<int> ParseCsvTiles(const char* csvText, int expectedCount);

    int width_ = 0;
    int height_ = 0;
    int tileWidth_ = 0;
    int tileHeight_ = 0;

    std::vector<TileLayer> layers_;
    std::vector<MapEvent> events_;
};

} // namespace World
