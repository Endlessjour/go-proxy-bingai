#pragma once

#include <cstdint>
#include <functional>

namespace Core {

class GameLoop {
public:
    using TickCallback = std::function<void()>;
    using UpdateCallback = std::function<void(double)>;

    void Run(const TickCallback& input, const UpdateCallback& update, const TickCallback& render, const std::function<bool()>& running);

private:
    static constexpr std::uint32_t kTargetFps = 60;
    static constexpr double kFixedDeltaTime = 1.0 / static_cast<double>(kTargetFps);
    static constexpr std::uint32_t kFrameMs = 1000 / kTargetFps;
};

} // namespace Core
