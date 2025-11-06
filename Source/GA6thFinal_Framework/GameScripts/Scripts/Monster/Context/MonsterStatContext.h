#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    struct StatContext
    {
        int     LevelID     = 0;  // 레벨 ID
        DataID  MonsterID   = 0;  // 몬스터 ID
        int     Health      = 0;  // 몬스터 체력
        int     StunResist  = 0;  // 몬스터 기절 저항
        int     Speed       = 0;  // 몬스터 속도
        std::vector<StatParam>                                  StatParams;     // 스탯 파라미터
        std::array<std::vector<ActionParam>, MAX_SKILL_COUNT>   ActionParams;   // 액션 파라미터
        std::array<std::vector<TokenParam>,  MAX_SKILL_COUNT>   TokenParams;    // 토큰 파라미터
    };
};