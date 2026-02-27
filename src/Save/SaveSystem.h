#pragma once

#include "Monster/MonsterInstance.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Save {

struct PlayerPosition {
    std::string map;
    int x = 0;
    int y = 0;
};

struct SaveData {
    PlayerPosition playerPosition;
    std::vector<Monster::MonsterInstance> party;
    std::unordered_map<std::string, bool> flags;
};

class SaveSystem {
public:
    bool SaveGame(const std::string& path, const SaveData& data) const;
    bool LoadGame(const std::string& path, SaveData& outData) const;
};

} // namespace Save
