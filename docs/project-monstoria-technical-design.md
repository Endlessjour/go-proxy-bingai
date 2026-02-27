# Project Monstoria 技术设计文档（MVP）

> 目标：基于 C++20 + SDL2 + Lua + JSON + TMX 构建可持续扩展的 2D 回合制怪物收集 RPG 框架。

## 1. 架构总览

```text
Game
├── Core        # 生命周期、时间、事件总线、资源定位
├── Data        # JSON/TMX 加载，Schema 校验，数据库索引
├── Render      # SDL2 渲染封装，摄像机与图层绘制
├── World       # 地图、碰撞、触发器、遭遇区域
├── Entity      # 玩家/NPC/怪物运行时实体
├── Battle      # 独立回合制状态机
├── Script      # Lua VM + C++ API 绑定
├── Save        # 快照序列化/反序列化
└── UI          # 游戏 UI 与 Dear ImGui 调试面板
```

核心原则：
- 逻辑层（World/Battle/Data）不依赖 UI 与渲染细节。
- 所有内容来源于 `data/`，禁止把怪物、技能、地图事件写死在代码中。
- 状态机节点可序列化，确保存档后可无损恢复。

## 2. 推荐目录结构

```text
monstoria/
├── CMakeLists.txt
├── src/
│   ├── core/
│   ├── data/
│   ├── render/
│   ├── world/
│   ├── entity/
│   ├── battle/
│   ├── script/
│   ├── save/
│   ├── ui/
│   └── main.cpp
├── include/
│   └── monstoria/
├── data/
│   ├── species/
│   ├── moves/
│   ├── maps/
│   ├── encounters/
│   ├── scripts/
│   └── localization/
└── tools/
    └── validation/
```

## 3. 数据驱动模型

### 3.1 Species（物种）与 Instance（个体）分离

- `SpeciesDef`：静态配置（基础种族值、可学技能、属性）。
- `MonsterInstance`：动态状态（等级、经验、个体值、努力值、异常状态、已学技能）。

```cpp
struct Stats {
    int hp;
    int atk;
    int def;
    int spAtk;
    int spDef;
    int spd;
};

struct MoveInstance {
    uint16_t moveId;
    uint8_t  pp;
    uint8_t  maxPp;
};

struct MonsterInstance {
    uint32_t uid;
    uint16_t speciesId;
    uint8_t level;
    uint32_t exp;
    Stats iv;
    Stats ev;
    Stats calculated;
    std::vector<MoveInstance> moves;
    uint16_t status;
};
```

### 3.2 数据加载流程

1. `DataRegistry` 扫描并加载 `data/`。
2. 使用 nlohmann/json 做结构化反序列化。
3. 在启动阶段执行 schema/引用校验：
   - species 是否引用了存在的 move。
   - map 配置引用的 tmx / encounter 表是否存在。
4. 建立 ID -> Def 哈希索引，提供只读查询接口。

## 4. 地图与世界系统

### 4.1 地图分层

- `TileLayer`: 地形渲染
- `CollisionLayer`: 阻挡判定
- `EncounterLayer`: 遭遇区域
- `EventLayer`: 脚本触发器
- `ObjectLayer`: NPC/传送点

### 4.2 World Update 流程

1. 输入系统更新玩家意图。
2. 世界系统进行移动与碰撞检测。
3. 进入触发器时，投递脚本事件。
4. 处于遭遇层时执行遇敌判定：

```cpp
if (rng.RollPercent(encounterRate)) {
    auto wild = encounterTable.RollWeighted(rng);
    gameState.PushScene(SceneType::Battle, wild);
}
```

## 5. 战斗系统（独立状态机）

战斗状态机：

```text
BattleInit
→ SendOut
→ CommandSelect
→ ActionQueueResolve
→ DamageApply
→ FaintCheck
→ TurnEnd
→ BattleEnd
```

### 5.1 指令队列

```cpp
enum class CommandType { Move, Switch, Item, Escape };

struct BattleCommand {
    CommandType type;
    int actor;
    int target;
    int moveId;
    int priority;
};
```

结算顺序：
1. 收集双方行动。
2. 按优先级 + 速度排序。
3. 逐条执行并记录日志事件（用于 UI 回放）。
4. 应用持续状态，进入回合结束。

> 建议：Battle 仅输出 `BattleEvent` 列表，UI 订阅并做动画，不反向驱动战斗逻辑。

## 6. Lua 脚本边界

Lua 负责：
- 对话
- NPC 行为
- 地图事件

Lua 不负责：
- 伤害公式
- 命中判定
- 速度排序

典型绑定 API：
- `ShowDialogue(text)`
- `GiveMonster(speciesId, level)`
- `SetFlag(flagId, value)`
- `WarpTo(mapId, x, y)`

## 7. 存档系统

存档对象必须是完整快照：

```text
SaveData
├── PlayerState
├── PartyState
├── WorldFlags
├── MapStates
├── Inventory
└── ActiveSceneState (World/Battle)
```

关键点：
- 存档应包含当前状态机节点。
- 战斗中存档恢复后，指令队列和随机数状态必须一致。
- 建议保存 RNG seed + consume counter，保证确定性重放。

## 8. 资源管理

统一缓存：
- `TextureHandle GetTexture(id)`
- `SoundHandle GetSound(id)`
- `FontHandle GetFont(id)`

策略：
- 引用计数或句柄表，避免重复加载。
- 切图/图集元数据也走 JSON。
- 地图切换触发“预加载 + 逐步释放”。

## 9. MVP 里程碑（建议）

### M1：可行走世界
- 地图加载（TMX）
- 玩家移动与碰撞
- NPC 静态对话

### M2：基础战斗闭环
- 随机遭遇
- 战斗状态机最小实现（攻击/受伤/击倒）
- 战斗结束回到世界

### M3：捕捉与队伍
- 捕捉流程
- 队伍上限与切换
- 基础背包系统

### M4：存档一致性
- 世界内存档/读档
- 战斗后状态一致恢复
- 数据热重载（开发模式）

## 10. 质量与可测试性

建议建立以下自动化测试：
- `BattleDeterminismTest`: 固定 seed 下 1000 场战斗日志一致。
- `DataValidationTest`: 全量 JSON 与引用校验。
- `SaveLoadRoundTripTest`: 存档读档后二进制/语义一致。
- `EncounterWeightTest`: 随机分布接近期望权重。

## 11. 非目标约束（执行层面）

- 不做联网。
- 不做实时战斗。
- 不做 3D。
- 不做程序生成地图。

该文档可作为工程初始化时的“架构合同”：先完成 MVP 再扩展剧情与内容体量。
