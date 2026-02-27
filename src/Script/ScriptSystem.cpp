#include "Script/ScriptSystem.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace Script {

bool ScriptSystem::Init() {
    lua_.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table);
    BindApi();
    return true;
}

bool ScriptSystem::LoadScripts(const std::string& directoryPath) {
    scriptSources_.clear();

    const std::filesystem::path dir(directoryPath);
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return false;
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".lua") {
            continue;
        }

        std::ifstream input(entry.path());
        if (!input.is_open()) {
            continue;
        }

        std::stringstream buffer;
        buffer << input.rdbuf();
        scriptSources_[entry.path().filename().string()] = buffer.str();
    }

    return true;
}

void ScriptSystem::SetShowTextCallback(ShowTextCallback callback) {
    showTextCallback_ = std::move(callback);
}

void ScriptSystem::SetStartWildBattleCallback(StartWildBattleCallback callback) {
    startWildBattleCallback_ = std::move(callback);
}

void ScriptSystem::SetGiveMonsterCallback(GiveMonsterCallback callback) {
    giveMonsterCallback_ = std::move(callback);
}

bool ScriptSystem::RunInteractScript(const std::string& scriptName) {
    auto it = scriptSources_.find(scriptName);
    if (it == scriptSources_.end()) {
        return false;
    }

    const sol::load_result loaded = lua_.load(it->second);
    if (!loaded.valid()) {
        return false;
    }

    const sol::protected_function_result scriptExec = loaded();
    if (!scriptExec.valid()) {
        return false;
    }

    sol::protected_function onInteract = lua_["OnInteract"];
    if (!onInteract.valid()) {
        return false;
    }

    const sol::protected_function_result callResult = onInteract();
    return callResult.valid();
}

void ScriptSystem::BindApi() {
    lua_.set_function("ShowText", [this](const std::string& text) {
        if (showTextCallback_) {
            showTextCallback_(text);
        }
    });

    lua_.set_function("StartWildBattle", [this](int species, int level) {
        if (startWildBattleCallback_) {
            startWildBattleCallback_(species, level);
        }
    });

    lua_.set_function("GiveMonster", [this](int species, int level) {
        if (giveMonsterCallback_) {
            giveMonsterCallback_(species, level);
        }
    });
}

} // namespace Script
