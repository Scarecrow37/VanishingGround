#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    struct DataContext
    {
        DataID                  ID          = 0;    // 몬스터 ID
        std::string             Name        = "";   // 몬스터 이름
        int                     PrefabID    = 0;    // 몬스터 프리팹 ID
        std::vector<FSMID>      FsmIDs;             // AI FSM ID
        std::vector<ActionID>   ActionIDs;          // Action ID
    };
};