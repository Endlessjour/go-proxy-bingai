#pragma once

#include "Battle/MonsterInstance.h"
#include "Capture/CaptureSystem.h"
#include "Capture/PlayerParty.h"

namespace Data {
class MoveDatabase;
}

namespace Battle {

enum class BattleState {
    Init,
    StartTurn,
    CommandSelect,
    Resolve,
    EndTurn,
    Finished
};

enum class BattleCommandType {
    UseMove,
    UseItem
};

enum class BattleItemType {
    CaptureBall
};

struct BattleCommand {
    BattleCommandType type = BattleCommandType::UseMove;
    bool actorIsPlayer = true;
    int moveId = 0;
    BattleItemType itemType = BattleItemType::CaptureBall;
};

class BattleSystem {
public:
    void SetMoveDatabase(const Data::MoveDatabase* moveDatabase);
    void StartWildBattle(const MonsterInstance& wildMonster);
    void Tick();

    bool IsRunning() const;
    BattleState GetState() const { return state_; }

private:
    void TransitionTo(BattleState next);
    void SelectCommands();
    void ResolveCommands();
    void EnsurePlayerMonster();

    const Data::MoveDatabase* moveDatabase_ = nullptr;
    Capture::CaptureSystem captureSystem_{};
    Capture::PlayerParty playerParty_{};
    BattleState state_ = BattleState::Finished;
    MonsterInstance playerMonster_{};
    MonsterInstance activeWild_{};
    BattleCommand playerCommand_{};
    BattleCommand wildCommand_{};
    int turnCount_ = 0;
};

} // namespace Battle
