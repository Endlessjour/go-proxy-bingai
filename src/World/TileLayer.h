#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace World {

class TileLayer {
public:
    TileLayer() = default;
    TileLayer(std::string name, int width, int height, std::vector<int> tiles);

    const std::string& GetName() const { return name_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

    int GetTile(int x, int y) const;
    bool IsBlocked(int x, int y) const;

private:
    std::string name_;
    int width_ = 0;
    int height_ = 0;
    std::vector<int> tiles_;
};

} // namespace World
