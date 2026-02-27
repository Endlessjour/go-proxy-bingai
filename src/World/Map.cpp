#include "World/Map.h"

#include <tinyxml2.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <utility>

namespace World {

namespace {

SDL_Color ColorFromGid(int gid) {
    if (gid <= 0) {
        return SDL_Color{0, 0, 0, 0};
    }

    const std::uint8_t r = static_cast<std::uint8_t>((gid * 53) % 255);
    const std::uint8_t g = static_cast<std::uint8_t>((gid * 97) % 255);
    const std::uint8_t b = static_cast<std::uint8_t>((gid * 151) % 255);
    return SDL_Color{r, g, b, 255};
}

bool IsCollisionLayerName(const std::string& name) {
    std::string lowered = name;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return lowered == "collision" || lowered == "collisionlayer" || lowered == "blocked";
}

bool IsEncounterLayerName(const std::string& name) {
    std::string lowered = name;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return lowered == "encounter" || lowered == "encounterlayer";
}

} // namespace

TileLayer::TileLayer(std::string name, int width, int height, std::vector<int> tiles)
    : name_(std::move(name)), width_(width), height_(height), tiles_(std::move(tiles)) {}

int TileLayer::GetTile(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        return 0;
    }

    return tiles_[static_cast<std::size_t>(y * width_ + x)];
}

bool TileLayer::IsBlocked(int x, int y) const {
    return GetTile(x, y) != 0;
}

bool Map::LoadFromTmx(const std::string& tmxPath) {
    tinyxml2::XMLDocument document;
    if (document.LoadFile(tmxPath.c_str()) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    const tinyxml2::XMLElement* mapNode = document.FirstChildElement("map");
    if (mapNode == nullptr) {
        return false;
    }

    width_ = mapNode->IntAttribute("width");
    height_ = mapNode->IntAttribute("height");
    tileWidth_ = mapNode->IntAttribute("tilewidth");
    tileHeight_ = mapNode->IntAttribute("tileheight");

    if (width_ <= 0 || height_ <= 0 || tileWidth_ <= 0 || tileHeight_ <= 0) {
        return false;
    }

    layers_.clear();
    events_.clear();

    for (const tinyxml2::XMLElement* layerNode = mapNode->FirstChildElement("layer");
         layerNode != nullptr;
         layerNode = layerNode->NextSiblingElement("layer")) {
        const char* layerName = layerNode->Attribute("name");
        const int layerWidth = layerNode->IntAttribute("width", width_);
        const int layerHeight = layerNode->IntAttribute("height", height_);

        const tinyxml2::XMLElement* dataNode = layerNode->FirstChildElement("data");
        if (dataNode == nullptr) {
            continue;
        }

        const char* encoding = dataNode->Attribute("encoding");
        if (encoding == nullptr || std::string(encoding) != "csv") {
            continue;
        }

        const char* csv = dataNode->GetText();
        if (csv == nullptr) {
            continue;
        }

        std::vector<int> tiles = ParseCsvTiles(csv, layerWidth * layerHeight);
        if (static_cast<int>(tiles.size()) != layerWidth * layerHeight) {
            continue;
        }

        std::string storedName = (layerName != nullptr) ? layerName : "TileLayer";
        layers_.emplace_back(storedName, layerWidth, layerHeight, std::move(tiles));
    }


    for (const tinyxml2::XMLElement* objectGroup = mapNode->FirstChildElement("objectgroup");
         objectGroup != nullptr;
         objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
        const char* groupName = objectGroup->Attribute("name");
        if (groupName == nullptr || std::string(groupName) != "EventLayer") {
            continue;
        }

        for (const tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object");
             object != nullptr;
             object = object->NextSiblingElement("object")) {
            const int tileX = static_cast<int>(object->FloatAttribute("x", 0.0f)) / tileWidth_;
            const int tileY = static_cast<int>(object->FloatAttribute("y", 0.0f)) / tileHeight_;

            std::string scriptName;
            const tinyxml2::XMLElement* properties = object->FirstChildElement("properties");
            if (properties != nullptr) {
                for (const tinyxml2::XMLElement* property = properties->FirstChildElement("property");
                     property != nullptr;
                     property = property->NextSiblingElement("property")) {
                    const char* name = property->Attribute("name");
                    if (name == nullptr || std::string(name) != "script") {
                        continue;
                    }

                    const char* value = property->Attribute("value");
                    if (value != nullptr) {
                        scriptName = value;
                    } else if (property->GetText() != nullptr) {
                        scriptName = property->GetText();
                    }
                    break;
                }
            }

            if (!scriptName.empty()) {
                events_.push_back(MapEvent{tileX, tileY, scriptName});
            }
        }
    }

    return !layers_.empty();
}

void Map::Render(SDL_Renderer* renderer) const {
    if (renderer == nullptr) {
        return;
    }

    for (const TileLayer& layer : layers_) {
        if (IsCollisionLayerName(layer.GetName()) || IsEncounterLayerName(layer.GetName())) {
            continue;
        }

        for (int y = 0; y < layer.GetHeight(); ++y) {
            for (int x = 0; x < layer.GetWidth(); ++x) {
                const int gid = layer.GetTile(x, y);
                if (gid == 0) {
                    continue;
                }

                const SDL_Color color = ColorFromGid(gid);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

                SDL_Rect rect{
                    x * tileWidth_,
                    y * tileHeight_,
                    tileWidth_,
                    tileHeight_
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

bool Map::IsBlocked(int tileX, int tileY) const {
    if (tileX < 0 || tileY < 0 || tileX >= width_ || tileY >= height_) {
        return true;
    }

    for (const TileLayer& layer : layers_) {
        if (!IsCollisionLayerName(layer.GetName())) {
            continue;
        }

        if (layer.IsBlocked(tileX, tileY)) {
            return true;
        }
    }

    return false;
}

bool Map::IsEncounterTile(int tileX, int tileY) const {
    if (tileX < 0 || tileY < 0 || tileX >= width_ || tileY >= height_) {
        return false;
    }

    for (const TileLayer& layer : layers_) {
        if (!IsEncounterLayerName(layer.GetName())) {
            continue;
        }

        if (layer.GetTile(tileX, tileY) != 0) {
            return true;
        }
    }

    return false;
}

std::optional<MapEvent> Map::GetEventAt(int tileX, int tileY) const {
    for (const auto& event : events_) {
        if (event.tileX == tileX && event.tileY == tileY) {
            return event;
        }
    }

    return std::nullopt;
}

std::vector<int> Map::ParseCsvTiles(const char* csvText, int expectedCount) {
    std::vector<int> tiles;
    tiles.reserve(static_cast<std::size_t>(expectedCount));

    std::stringstream stream(csvText);
    std::string cell;
    while (std::getline(stream, cell, ',')) {
        std::stringstream valueStream(cell);
        int value = 0;
        valueStream >> value;
        tiles.push_back(value);
    }

    return tiles;
}

} // namespace World
