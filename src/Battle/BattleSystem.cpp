#include "Battle/BattleSystem.h"

#include "Data/MoveDatabase.h"

#include <algorithm>
#include <iostream>

namespace Battle {

namespace {

const char* ToString(BattleState state) {
    switch (state) {
    case BattleState::Init:
        return "Init";
    case BattleState::StartTurn:
        return "StartTurn";
    case BattleState::CommandSelect:
        return "CommandSelect";
    case BattleState::Resolve:
        return "Resolve";
    case BattleState::EndTurn:
        return "EndTurn";
    case BattleState::Finished:
        return "Finished";
    default:
        return "Unknown";
    }
}

int SelectUsableMoveId(const MonsterInstance& monster) {
    for (const auto& move : monster.moves) {
        if (move.currentPP > 0) {
            return move.moveId;
        }
    }
    return 0;
}

void ConsumeMovePP(MonsterInstance& monster, int moveId) {
    for (auto& move : monster.moves) {
        if (move.moveId == moveId && move.currentPP > 0) {
            --move.currentPP;
            return;
        }
    }
}

} // namespace

void BattleSystem::SetMoveDatabase(const Data::MoveDatabase* moveDatabase) {
    moveDatabase_ = moveDatabase;
}

void BattleSystem::SetPlayerParty(Capture::PlayerParty* playerParty) {
    playerParty_ = playerParty;
}

void BattleSystem::StartWildBattle(const MonsterInstance& wildMonster) {
    activeWild_ = wildMonster;
    turnCount_ = 0;
    EnsurePlayerMonster();

    std::cout << "Battle started: wild species=" << activeWild_.speciesId
              << " level=" << static_cast<int>(activeWild_.level) << '\n';

    TransitionTo(BattleState::Init);
}

void BattleSystem::Tick() {
    if (!IsRunning()) {
        return;
    }

    switch (state_) {
    case BattleState::Init:
        TransitionTo(BattleState::StartTurn);
        break;
    case BattleState::StartTurn:
        ++turnCount_;
        std::cout << "Battle turn=" << turnCount_ << '\n';
        TransitionTo(BattleState::CommandSelect);
        break;
    case BattleState::CommandSelect:
        SelectCommands();
        TransitionTo(BattleState::Resolve);
        break;
    case BattleState::Resolve:
        ResolveCommands();
        TransitionTo(BattleState::EndTurn);
        break;
    case BattleState::EndTurn:
        if (playerMonster_.currentHp <= 0 || activeWild_.currentHp <= 0) {
            TransitionTo(BattleState::Finished);
        } else {
            TransitionTo(BattleState::StartTurn);
        }
        break;
    case BattleState::Finished:
        break;
    }
}

bool BattleSystem::IsRunning() const {
    return state_ != BattleState::Finished;
}

void BattleSystem::TransitionTo(BattleState next) {
    std::cout << "Battle state: " << ToString(state_) << " -> " << ToString(next) << '\n';
    state_ = next;
}

void BattleSystem::SelectCommands() {
    playerCommand_.actorIsPlayer = true;

    if (activeWild_.currentHp <= (activeWild_.maxHp / 2)) {
        playerCommand_.type = BattleCommandType::UseItem;
        playerCommand_.itemType = BattleItemType::CaptureBall;
        playerCommand_.moveId = 0;
    } else {
        playerCommand_.type = BattleCommandType::UseMove;
        playerCommand_.moveId = SelectUsableMoveId(playerMonster_);
    }

    wildCommand_.type = BattleCommandType::UseMove;
    wildCommand_.actorIsPlayer = false;
    wildCommand_.moveId = SelectUsableMoveId(activeWild_);

    std::cout << "CommandSelect: player="
              << (playerCommand_.type == BattleCommandType::UseItem ? "CaptureBall" : "UseMove")
              << " wild move=" << wildCommand_.moveId << '\n';
}

void BattleSystem::ResolveCommands() {
    if (playerCommand_.type == BattleCommandType::UseItem &&
        playerCommand_.itemType == BattleItemType::CaptureBall) {
        const double ballRate = 1.0;
        const bool captured = captureSystem_.TryCapture(activeWild_, ballRate);
        if (captured) {
            if (playerParty_ != nullptr && playerParty_->Add(activeWild_)) {
                std::cout << "Capture success: species=" << activeWild_.speciesId
                          << " party_size=" << playerParty_->Size() << '/' << playerParty_->MaxSize() << '\n';
                activeWild_.currentHp = 0;
            } else {
                std::cout << "Capture failed: party full" << '\n';
            }
        } else {
            std::cout << "Capture failed: species=" << activeWild_.speciesId << '\n';
        }
    }

    auto applyUseMove = [this](BattleCommand command) {
        MonsterInstance& attacker = command.actorIsPlayer ? playerMonster_ : activeWild_;
        MonsterInstance& defender = command.actorIsPlayer ? activeWild_ : playerMonster_;

        if (command.type != BattleCommandType::UseMove || command.moveId == 0 || moveDatabase_ == nullptr) {
            return;
        }

        auto moveOpt = moveDatabase_->GetMove(command.moveId);
        if (!moveOpt.has_value()) {
            return;
        }

        ConsumeMovePP(attacker, command.moveId);
        const int damage = moveOpt->power + static_cast<int>(attacker.level);
        defender.currentHp = std::max(0, defender.currentHp - damage);

        std::cout << "UseMove: actor=" << (command.actorIsPlayer ? "Player" : "Wild")
                  << " move=" << moveOpt->name
                  << " damage=" << damage
                  << " targetHP=" << defender.currentHp << '/' << defender.maxHp << '\n';
    };

    if (activeWild_.currentHp > 0) {
        applyUseMove(playerCommand_);
    }
    if (activeWild_.currentHp > 0) {
        applyUseMove(wildCommand_);
    }

    SyncPlayerMonsterToParty();
}

void BattleSystem::EnsurePlayerMonster() {
    if (playerParty_ != nullptr) {
        const auto& members = playerParty_->GetMembers();
        for (std::size_t i = 0; i < members.size(); ++i) {
            if (members[i].currentHp > 0) {
                playerMonster_ = members[i];
                activePartyIndex_ = i;
                return;
            }
        }
    }

    playerMonster_.uid = 999001;
    playerMonster_.speciesId = 1;
    playerMonster_.level = 5;
    playerMonster_.maxHp = 50;
    playerMonster_.currentHp = playerMonster_.maxHp;

    if (playerMonster_.moves.empty() && moveDatabase_ != nullptr) {
        const int moveId = moveDatabase_->GetFirstMoveId();
        if (moveId > 0) {
            playerMonster_.moves.push_back({moveId, 25});
        }
    }

    activePartyIndex_ = 0;
}

void BattleSystem::SyncPlayerMonsterToParty() {
    if (playerParty_ == nullptr) {
        return;
    }

    auto members = playerParty_->GetMembers();
    if (activePartyIndex_ >= members.size()) {
        return;
    }

    members[activePartyIndex_] = playerMonster_;
    playerParty_->SetMembers(members);
}

} // namespace Battle
