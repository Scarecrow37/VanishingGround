#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    struct DataContext
    {
        DataID              ID          = 0;                // 몬스터 ID
        std::string         Name        = "";               // 몬스터 이름
        int                 PrefabID    = 0;                // 몬스터 프리팹 ID
        FSMArray            FsmIDs      = {0, 0, 0};        // AI FSM ID
        SkillArray          ActionIDs   = {0, 0, 0, 0, 0};  // Action ID
    };
};