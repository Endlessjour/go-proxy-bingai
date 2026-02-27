#include "Capture/PlayerParty.h"

namespace Capture {

bool PlayerParty::Add(const Monster::MonsterInstance& monster) {
    if (members_.size() >= maxSize_) {
        return false;
    }

    members_.push_back(monster);
    return true;
}

void PlayerParty::SetMembers(const std::vector<Monster::MonsterInstance>& monsters) {
    members_.clear();
    for (const auto& monster : monsters) {
        if (members_.size() >= maxSize_) {
            break;
        }
        members_.push_back(monster);
    }
}

} // namespace Capture
