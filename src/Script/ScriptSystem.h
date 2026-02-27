#pragma once

#include <sol/sol.hpp>

#include <functional>
#include <string>
#include <unordered_map>

namespace Script {

class ScriptSystem {
public:
    using ShowTextCallback = std::function<void(const std::string&)>;
    using StartWildBattleCallback = std::function<void(int, int)>;
    using GiveMonsterCallback = std::function<void(int, int)>;

    bool Init();
    bool LoadScripts(const std::string& directoryPath);

    void SetShowTextCallback(ShowTextCallback callback);
    void SetStartWildBattleCallback(StartWildBattleCallback callback);
    void SetGiveMonsterCallback(GiveMonsterCallback callback);

    bool RunInteractScript(const std::string& scriptName);

private:
    void BindApi();

    sol::state lua_;
    std::unordered_map<std::string, std::string> scriptSources_;
    ShowTextCallback showTextCallback_;
    StartWildBattleCallback startWildBattleCallback_;
    GiveMonsterCallback giveMonsterCallback_;
};

} // namespace Script
