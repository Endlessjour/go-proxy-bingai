#include "Game/Game.h"

#include <SDL.h>

#include <iostream>

namespace Game {

Game::Game(Render::RenderSystem& renderSystem)
    : renderSystem_(renderSystem),
      monsterFactory_(speciesDatabase_) {
    map_.LoadFromTmx("data/maps/phase2_demo.tmx");
    player_.SetSpawn(2, 2);
    encounterSystem_.LoadFromFile("data/encounters/route01.json");
    speciesDatabase_.LoadDirectory("data/species");
    moveDatabase_.LoadDirectory("data/moves");

    scriptSystem_.Init();
    scriptSystem_.LoadScripts("data/scripts");
    scriptSystem_.SetShowTextCallback([](const std::string& text) {
        std::cout << "Dialogue: " << text << '\n';
    });
    scriptSystem_.SetStartWildBattleCallback([this](int species, int level) {
        StartWildBattleFromScript(species, level);
    });
    scriptSystem_.SetGiveMonsterCallback([this](int species, int level) {
        GiveMonsterFromScript(species, level);
    });

    battleSystem_.SetMoveDatabase(&moveDatabase_);
    lastPlayerTileX_ = player_.GetTileX();
    lastPlayerTileY_ = player_.GetTileY();
}

void Game::HandleInput() {
    if (battleSystem_.IsRunning()) {
        return;
    }

    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    const bool savePressed = keys[SDL_SCANCODE_F5] != 0;
    if (savePressed && !saveHeld_) {
        SaveGame();
    }
    saveHeld_ = savePressed;

    const bool loadPressed = keys[SDL_SCANCODE_F9] != 0;
    if (loadPressed && !loadHeld_) {
        LoadGame();
    }
    loadHeld_ = loadPressed;

    const bool interactPressed = keys[SDL_SCANCODE_E] != 0;
    if (interactPressed && !interactHeld_) {
        const auto eventOpt = map_.GetEventAt(player_.GetTileX(), player_.GetTileY());
        if (eventOpt.has_value()) {
            scriptSystem_.RunInteractScript(eventOpt->script);
        }
    }
    interactHeld_ = interactPressed;

    if (player_.IsMoving()) {
        return;
    }

    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
        player_.TryMove(0, -1, map_);
    } else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
        player_.TryMove(0, 1, map_);
    } else if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
        player_.TryMove(-1, 0, map_);
    } else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
        player_.TryMove(1, 0, map_);
    }
}

void Game::Update(double fixedDeltaTime) {
    if (battleSystem_.IsRunning()) {
        battleSystem_.Tick();
        return;
    }

    player_.Update(fixedDeltaTime, map_.GetTileWidth(), map_.GetTileHeight());

    if (player_.IsMoving()) {
        return;
    }

    const int currentX = player_.GetTileX();
    const int currentY = player_.GetTileY();
    const bool enteredNewTile = currentX != lastPlayerTileX_ || currentY != lastPlayerTileY_;

    if (enteredNewTile && map_.IsEncounterTile(currentX, currentY) && encounterSystem_.RollEncounter()) {
        int species = 0;
        int level = 0;
        if (encounterSystem_.SelectEncounter(species, level)) {
            std::cout << "Encounter: species=" << species << " level=" << level << '\n';
            StartWildBattleFromScript(species, level);
        }
    }

    lastPlayerTileX_ = currentX;
    lastPlayerTileY_ = currentY;
}

void Game::Render() {
    renderSystem_.BeginFrame();
    map_.Render(renderSystem_.GetRenderer());
    player_.Render(renderSystem_.GetRenderer(), map_.GetTileWidth(), map_.GetTileHeight());
    renderSystem_.EndFrame();
}

void Game::StartWildBattleFromScript(int species, int level) {
    auto wildMonster = monsterFactory_.Create(static_cast<std::uint16_t>(species), static_cast<std::uint8_t>(level));
    if (!wildMonster.has_value()) {
        return;
    }

    const int moveId = moveDatabase_.GetFirstMoveId();
    if (moveId > 0) {
        wildMonster->moves.push_back({moveId, 25});
    }

    battleSystem_.StartWildBattle(wildMonster.value());
}

void Game::GiveMonsterFromScript(int species, int level) {
    auto monster = monsterFactory_.Create(static_cast<std::uint16_t>(species), static_cast<std::uint8_t>(level));
    if (!monster.has_value()) {
        return;
    }

    const bool added = playerParty_.Add(monster.value());
    if (added) {
        worldFlags_["intro_complete"] = true;
    }

    std::cout << "GiveMonster: species=" << species << " level=" << level
              << (added ? " success" : " failed") << '\n';
}

void Game::SaveGame() {
    Save::SaveData data;
    data.playerPosition.map = currentMapId_;
    data.playerPosition.x = player_.GetTileX();
    data.playerPosition.y = player_.GetTileY();
    data.party = playerParty_.GetMembers();
    data.flags = worldFlags_;

    const bool ok = saveSystem_.SaveGame("save/save01.json", data);
    std::cout << (ok ? "SaveGame: success" : "SaveGame: failed") << '\n';
}

void Game::LoadGame() {
    Save::SaveData data;
    const bool ok = saveSystem_.LoadGame("save/save01.json", data);
    if (!ok) {
        std::cout << "LoadGame: failed" << '\n';
        return;
    }

    currentMapId_ = data.playerPosition.map;
    player_.SetSpawn(data.playerPosition.x, data.playerPosition.y);
    playerParty_.SetMembers(data.party);
    worldFlags_ = data.flags;

    lastPlayerTileX_ = player_.GetTileX();
    lastPlayerTileY_ = player_.GetTileY();

    std::cout << "LoadGame: success" << '\n';
}

} // namespace Game
