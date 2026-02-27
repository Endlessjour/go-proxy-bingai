#pragma once

#include "Move/MoveInstance.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <vector>

namespace Monster {

struct IVStats {
    int hp = 0;
    int atk = 0;
    int def = 0;
    int spd = 0;
};

struct MonsterInstance {
    std::uint32_t uid = 0;
    std::uint16_t speciesId = 0;
    std::uint8_t level = 1;
    IVStats iv;
    int maxHp = 1;
    int currentHp = 1;
    std::vector<Move::MoveInstance> moves;
};

inline void to_json(nlohmann::json& j, const IVStats& iv) {
    j = nlohmann::json{{"hp", iv.hp}, {"atk", iv.atk}, {"def", iv.def}, {"spd", iv.spd}};
}

inline void from_json(const nlohmann::json& j, IVStats& iv) {
    iv.hp = j.value("hp", 0);
    iv.atk = j.value("atk", 0);
    iv.def = j.value("def", 0);
    iv.spd = j.value("spd", 0);
}

inline void to_json(nlohmann::json& j, const MonsterInstance& monster) {
    j = nlohmann::json{
        {"uid", monster.uid},
        {"speciesId", monster.speciesId},
        {"level", monster.level},
        {"iv", monster.iv},
        {"maxHp", monster.maxHp},
        {"currentHp", monster.currentHp},
        {"moves", monster.moves}
    };
}

inline void from_json(const nlohmann::json& j, MonsterInstance& monster) {
    monster.uid = j.value("uid", 0u);
    monster.speciesId = j.value("speciesId", static_cast<std::uint16_t>(0));
    monster.level = j.value("level", static_cast<std::uint8_t>(1));
    monster.iv = j.value("iv", IVStats{});
    monster.maxHp = j.value("maxHp", 1);
    monster.currentHp = j.value("currentHp", monster.maxHp);
    monster.moves = j.value("moves", std::vector<Move::MoveInstance>{});
}

} // namespace Monster
