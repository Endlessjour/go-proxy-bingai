#pragma once

#include "Monster/MonsterInstance.h"

#include <cstddef>
#include <vector>

namespace Capture {

class PlayerParty {
public:
    bool Add(const Monster::MonsterInstance& monster);
    void SetMembers(const std::vector<Monster::MonsterInstance>& monsters);

    const std::vector<Monster::MonsterInstance>& GetMembers() const { return members_; }
    std::size_t Size() const { return members_.size(); }
    std::size_t MaxSize() const { return maxSize_; }

private:
    std::vector<Monster::MonsterInstance> members_;
    std::size_t maxSize_ = 6;
};

} // namespace Capture
