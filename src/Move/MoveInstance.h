#pragma once

#include <nlohmann/json.hpp>

namespace Move {

struct MoveInstance {
    int moveId = 0;
    int currentPP = 0;
};

inline void to_json(nlohmann::json& j, const MoveInstance& move) {
    j = nlohmann::json{
        {"moveId", move.moveId},
        {"currentPP", move.currentPP}
    };
}

inline void from_json(const nlohmann::json& j, MoveInstance& move) {
    move.moveId = j.value("moveId", 0);
    move.currentPP = j.value("currentPP", 0);
}

} // namespace Move
