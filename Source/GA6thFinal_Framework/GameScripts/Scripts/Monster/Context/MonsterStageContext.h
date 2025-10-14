#pragma once
#include "Monster/Common/MonsterCommon.h"

namespace Monster
{
    // 다음과 같은 형식으로 구성됨
    // ex) 205003:2, 205004:1 (토큰ID:개수)
    struct TokenParam
    {
        int TokenID = 0;    // 토큰 ID
        int Count   = 0;    // 토큰 개수
    };

    // 다음과 같은 형식으로 구성됨
    // ex) 1, 5, 2 (데미지1, 데미지2, 데미지3)
    struct ActionParam
    {
        int Param  = 0;    // 스킬 데미지
    };
    
    struct StageContext
    {
        int     StageID     = 0;  // 스테이지 ID
        DataID  MonsterID   = 0;  // 몬스터 ID
        int     Health      = 0;  // 몬스터 체력
        int     StunResist  = 0;  // 몬스터 기절 저항

        std::array<std::vector<ActionParam>, MAX_SKILL_COUNT> DamageParams; // 스킬 데미지 파라미터
        std::array<std::vector<TokenParam>,  MAX_SKILL_COUNT>   TokenParams;    // 스킬 토큰 파라미터
    };

    std::vector<ActionParam> ParseActionParam(const std::string& paramString);
    std::vector<TokenParam>  ParseTokenParam(const std::string& paramString);
};