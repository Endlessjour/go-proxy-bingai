#include "Monster/MonsterFactory.h"

#include <algorithm>

namespace Monster {

namespace {

int CalculateMaxHp(int baseHp, int ivHp, int level) {
    return std::max(1, baseHp + ivHp + level * 2);
}

} // namespace

MonsterFactory::MonsterFactory(const Data::SpeciesDatabase& speciesDatabase)
    : speciesDatabase_(speciesDatabase) {}

std::optional<MonsterInstance> MonsterFactory::Create(std::uint16_t speciesId, std::uint8_t level) {
    auto speciesOpt = speciesDatabase_.GetSpecies(speciesId);
    if (!speciesOpt.has_value()) {
        return std::nullopt;
    }

    std::uniform_int_distribution<int> ivRoll(0, 31);

    MonsterInstance monster;
    monster.uid = nextUid_++;
    monster.speciesId = speciesId;
    monster.level = std::max<std::uint8_t>(1, level);
    monster.iv.hp = ivRoll(rng_);
    monster.iv.atk = ivRoll(rng_);
    monster.iv.def = ivRoll(rng_);
    monster.iv.spd = ivRoll(rng_);

    const Data::Species& species = speciesOpt.value();
    monster.maxHp = CalculateMaxHp(species.baseStats.hp, monster.iv.hp, monster.level);
    monster.currentHp = monster.maxHp;

    return monster;
}

} // namespace Monster
