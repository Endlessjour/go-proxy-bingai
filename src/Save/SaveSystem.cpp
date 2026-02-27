#include "Save/SaveSystem.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace Save {

bool SaveSystem::SaveGame(const std::string& path, const SaveData& data) const {
    nlohmann::json root;
    root["player"]["position"]["map"] = data.playerPosition.map;
    root["player"]["position"]["x"] = data.playerPosition.x;
    root["player"]["position"]["y"] = data.playerPosition.y;
    root["party"] = data.party;
    root["flags"] = data.flags;

    const std::filesystem::path savePath(path);
    const auto parent = savePath.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    std::ofstream out(savePath);
    if (!out.is_open()) {
        return false;
    }

    out << root.dump(2);
    return true;
}

bool SaveSystem::LoadGame(const std::string& path, SaveData& outData) const {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    nlohmann::json root;
    input >> root;

    if (!root.contains("player") || !root.contains("party") || !root.contains("flags")) {
        return false;
    }

    outData.playerPosition.map = root["player"]["position"].value("map", "route01");
    outData.playerPosition.x = root["player"]["position"].value("x", 0);
    outData.playerPosition.y = root["player"]["position"].value("y", 0);
    outData.party = root.value("party", std::vector<Monster::MonsterInstance>{});
    outData.flags = root.value("flags", std::unordered_map<std::string, bool>{});

    return true;
}

} // namespace Save
