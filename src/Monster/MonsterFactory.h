#pragma once

#include "Data/Species.h"
#include "Monster/MonsterInstance.h"

#include <cstdint>
#include <optional>
#include <random>

namespace Monster {

class MonsterFactory {
public:
    explicit MonsterFactory(const Data::SpeciesDatabase& speciesDatabase);

    std::optional<MonsterInstance> Create(std::uint16_t speciesId, std::uint8_t level);

private:
    const Data::SpeciesDatabase& speciesDatabase_;
    std::mt19937 rng_{std::random_device{}()};
    std::uint32_t nextUid_ = 1;
};

} // namespace Monster
