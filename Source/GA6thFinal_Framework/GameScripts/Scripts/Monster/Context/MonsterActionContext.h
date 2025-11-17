#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    struct ActionContext
    {
        ActionID    ID          = 0;  // 액션 ID
        std::string Name        = ""; // 액션 이름
        int         IconID      = 0;  // 액션 아이콘 ID
        std::string Target      = ""; // 액션 타겟
        size_t      AttackCount = 0;  // 공격 횟수
        std::string Parameter   = ""; // 액션 파라미터
    };
} // namespace Monster
