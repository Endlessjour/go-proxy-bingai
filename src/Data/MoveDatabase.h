#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace Data {

struct MoveData {
    int id = 0;
    std::string name;
    int power = 0;
    int accuracy = 100;
    std::string type;
    std::string category;
    std::string target;
};

class MoveDatabase {
public:
    bool LoadDirectory(const std::string& directoryPath);
    std::optional<MoveData> GetMove(int moveId) const;
    int GetFirstMoveId() const;

private:
    std::unordered_map<int, MoveData> movesById_;
};

} // namespace Data
