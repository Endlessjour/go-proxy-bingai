#pragma once

#include "Monster/MonsterInstance.h"

#include <random>

namespace Capture {

class CaptureSystem {
public:
    bool TryCapture(const Monster::MonsterInstance& target, double ballRate);

private:
    std::mt19937 rng_{std::random_device{}()};
};

} // namespace Capture
