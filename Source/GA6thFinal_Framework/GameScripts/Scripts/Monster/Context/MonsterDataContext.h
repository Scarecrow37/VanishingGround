#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    struct DataContext
    {
        DataID          ID                                  = 0;                // 몬스터 ID
        std::string     Name                                = "";               // 몬스터 이름
        int             PrefabID                            = 0;                // 몬스터 프리팹 ID
        std::array<FSMID, MAX_FSM_COUNT>      FsmIDs        = {0, 0, 0};        // AI FSM ID
        std::array<ActionID, MAX_SKILL_COUNT> ActionIDs     = {0, 0, 0, 0, 0};  // Action ID
    };
};