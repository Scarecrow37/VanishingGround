#pragma once
#include "Monster/Common/MonsterCommon.h"
#include "MonsterStatContext.h"

namespace Monster
{
    struct MonsterSpawnContext
    {
        Monster::SpawnID SpawnID = 0; // 몬스터 스폰 ID
        Monster::LevelID LevelID = 0; // 스테이지 레벨 ID
        std::array<SpawnParam, Monster::MAX_ENEMY_COUNT> SpawnParams;
    };
};
