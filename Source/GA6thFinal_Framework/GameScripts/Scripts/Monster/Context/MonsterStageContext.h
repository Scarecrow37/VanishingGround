#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    struct SkillParam
    {
        std::string DamageParam;
        std::string TokenType;
        std::string TokenParam;
    };
    struct StageContext
    {
        int     StageID     = 0;  // 스테이지 ID
        DataID  MonsterID   = 0;  // 몬스터 ID
        int     Health      = 0;  // 몬스터 체력
        int     StunResist  = 0;  // 몬스터 기절 저항

        std::array<SkillParam, MAX_SKILL_COUNT> SkillParam; // 스킬 파라미터
    };
};
