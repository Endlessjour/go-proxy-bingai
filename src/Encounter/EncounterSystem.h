#pragma once

#include <random>
#include <string>
#include <vector>

namespace Encounter {

struct EncounterEntry {
    int species = 0;
    int weight = 0;
    int minLevel = 1;
    int maxLevel = 1;
};

class EncounterSystem {
public:
    bool LoadFromFile(const std::string& path);

    bool RollEncounter();
    bool SelectEncounter(int& outSpecies, int& outLevel);

private:
    double encounterRate_ = 0.0;
    std::vector<EncounterEntry> table_;
    std::mt19937 rng_{std::random_device{}()};
};

} // namespace Encounter
