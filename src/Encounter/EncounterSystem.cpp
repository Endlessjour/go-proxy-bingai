#include "Encounter/EncounterSystem.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <numeric>

namespace Encounter {

bool EncounterSystem::LoadFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    nlohmann::json root;
    input >> root;

    if (!root.contains("encounter_rate") || !root.contains("table")) {
        return false;
    }

    encounterRate_ = root["encounter_rate"].get<double>();
    table_.clear();

    for (const auto& item : root["table"]) {
        EncounterEntry entry;
        entry.species = item.value("species", 0);
        entry.weight = item.value("weight", 0);
        entry.minLevel = item.value("min_lv", 1);
        entry.maxLevel = item.value("max_lv", 1);

        if (entry.species > 0 && entry.weight > 0 && entry.minLevel > 0 && entry.maxLevel >= entry.minLevel) {
            table_.push_back(entry);
        }
    }

    return !table_.empty();
}

bool EncounterSystem::RollEncounter() {
    if (encounterRate_ <= 0.0) {
        return false;
    }

    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(rng_) < encounterRate_;
}

bool EncounterSystem::SelectEncounter(int& outSpecies, int& outLevel) {
    if (table_.empty()) {
        return false;
    }

    const int totalWeight = std::accumulate(table_.begin(), table_.end(), 0, [](int sum, const EncounterEntry& e) {
        return sum + e.weight;
    });

    if (totalWeight <= 0) {
        return false;
    }

    std::uniform_int_distribution<int> weightRoll(1, totalWeight);
    int pick = weightRoll(rng_);

    for (const EncounterEntry& entry : table_) {
        pick -= entry.weight;
        if (pick <= 0) {
            std::uniform_int_distribution<int> levelRoll(entry.minLevel, entry.maxLevel);
            outSpecies = entry.species;
            outLevel = levelRoll(rng_);
            return true;
        }
    }

    return false;
}

} // namespace Encounter
