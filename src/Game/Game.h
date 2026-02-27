#pragma once

#include "Battle/BattleSystem.h"
#include "Capture/PlayerParty.h"
#include "Data/MoveDatabase.h"
#include "Data/Species.h"
#include "Encounter/EncounterSystem.h"
#include "Entity/Player.h"
#include "Monster/MonsterFactory.h"
#include "Render/RenderSystem.h"
#include "Save/SaveSystem.h"
#include "Script/ScriptSystem.h"
#include "World/Map.h"

#include <string>
#include <unordered_map>

namespace Game {

class Game {
public:
    explicit Game(Render::RenderSystem& renderSystem);

    void HandleInput();
    void Update(double fixedDeltaTime);
    void Render();

private:
    void StartWildBattleFromScript(int species, int level);
    void GiveMonsterFromScript(int species, int level);
    void SaveGame();
    void LoadGame();

    Render::RenderSystem& renderSystem_;
    World::Map map_;
    Entity::Player player_;
    Encounter::EncounterSystem encounterSystem_;
    Data::SpeciesDatabase speciesDatabase_;
    Data::MoveDatabase moveDatabase_;
    Monster::MonsterFactory monsterFactory_;
    Script::ScriptSystem scriptSystem_;
    Save::SaveSystem saveSystem_;
    Capture::PlayerParty playerParty_;
    Battle::BattleSystem battleSystem_;
    int lastPlayerTileX_ = 0;
    int lastPlayerTileY_ = 0;
    bool interactHeld_ = false;
    bool saveHeld_ = false;
    bool loadHeld_ = false;
    std::string currentMapId_ = "route01";
    std::unordered_map<std::string, bool> worldFlags_{{"intro_complete", false}};
};

} // namespace Game
