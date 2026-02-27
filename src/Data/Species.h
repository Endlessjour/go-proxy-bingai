#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace Data {

struct BaseStats {
    int hp = 1;
    int atk = 1;
    int def = 1;
    int spd = 1;
};

struct Species {
    std::uint16_t id = 0;
    std::string name;
    BaseStats baseStats;
};

class SpeciesDatabase {
public:
    bool LoadDirectory(const std::string& directoryPath);
    std::optional<Species> GetSpecies(std::uint16_t speciesId) const;

private:
    std::unordered_map<std::uint16_t, Species> speciesById_;
};

} // namespace Data
