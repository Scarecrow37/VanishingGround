#pragma once
#include "Monster/Common/MonsterCommon.h"
#include "MonsterStatContext.h"

namespace Monster
{
    struct SpawnContext
    {
        SpawnID SpawnID = 0; // 몬스터 스폰 ID
        std::array<LevelID, MAX_DIFF_COUNT>     LevelID;     // 스테이지 레벨 ID
        std::array<SpawnParam, MAX_ENEMY_COUNT> SpawnParams;
    };
};
