#include "Data/MoveDatabase.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace Data {

bool MoveDatabase::LoadDirectory(const std::string& directoryPath) {
    movesById_.clear();

    const std::filesystem::path dir(directoryPath);
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return false;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }

        std::ifstream input(entry.path());
        if (!input.is_open()) {
            continue;
        }

        nlohmann::json root;
        input >> root;

        MoveData move;
        move.id = root.value("id", 0);
        move.name = root.value("name", "");
        move.power = root.value("power", 0);
        move.accuracy = root.value("accuracy", 100);
        move.type = root.value("type", "");
        move.category = root.value("category", "");
        move.target = root.value("target", "Single");

        if (move.id <= 0 || move.name.empty()) {
            continue;
        }

        movesById_[move.id] = move;
    }

    return !movesById_.empty();
}

std::optional<MoveData> MoveDatabase::GetMove(int moveId) const {
    auto it = movesById_.find(moveId);
    if (it == movesById_.end()) {
        return std::nullopt;
    }

    return it->second;
}

int MoveDatabase::GetFirstMoveId() const {
    if (movesById_.empty()) {
        return 0;
    }

    return movesById_.begin()->first;
}

} // namespace Data
