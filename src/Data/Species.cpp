#include "Data/Species.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace Data {

bool SpeciesDatabase::LoadDirectory(const std::string& directoryPath) {
    speciesById_.clear();

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

        Species species;
        species.id = root.value("id", 0);
        species.name = root.value("name", "");

        if (root.contains("base_stats")) {
            const auto& stats = root["base_stats"];
            species.baseStats.hp = stats.value("hp", 1);
            species.baseStats.atk = stats.value("atk", 1);
            species.baseStats.def = stats.value("def", 1);
            species.baseStats.spd = stats.value("spd", 1);
        }

        if (species.id == 0 || species.name.empty()) {
            continue;
        }

        speciesById_[species.id] = species;
    }

    return !speciesById_.empty();
}

std::optional<Species> SpeciesDatabase::GetSpecies(std::uint16_t speciesId) const {
    auto it = speciesById_.find(speciesId);
    if (it == speciesById_.end()) {
        return std::nullopt;
    }

    return it->second;
}

} // namespace Data
