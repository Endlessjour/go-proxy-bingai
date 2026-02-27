#include "Capture/CaptureSystem.h"

#include <algorithm>

namespace Capture {

bool CaptureSystem::TryCapture(const Monster::MonsterInstance& target, double ballRate) {
    if (target.maxHp <= 0) {
        return false;
    }

    const double hpFactor = static_cast<double>(target.maxHp - target.currentHp) / static_cast<double>(target.maxHp);
    const double catchChance = std::clamp(hpFactor * ballRate, 0.0, 1.0);

    std::uniform_real_distribution<double> roll(0.0, 1.0);
    return roll(rng_) < catchChance;
}

} // namespace Capture
