#pragma once

namespace Monster
{
    using FSMID         = int; // 몬스터 FSM ID
    using DataID        = int; // 몬스터 ID
    using ActionID      = int; // 몬스터 액션 ID
    using LevelID       = int; // 스테이지 레벨 ID
    using SpawnID       = int; // 몬스터 스폰 ID

    constexpr size_t MAX_ENEMY_COUNT = 3;
    constexpr size_t MAX_FSM_COUNT   = 3;
    constexpr size_t MAX_SKILL_COUNT = 5;
    
    constexpr const std::array<const char*, MAX_ENEMY_COUNT> SPAWN_POINT_TAGS = {
        "Enemy Spawn Point Left",
        "Enemy Spawn Point Middle",
        "Enemy Spawn Point Right"};

    struct TokenParam
    {
        int TokenID = 0; // 토큰 ID
        int Count   = 0; // 토큰 개수
    };
    struct ActionParam
    {
        int Param = 0; // 스킬 데미지
    };
    struct SpawnParam
    {
        DataID MonsterID = 0; // 몬스터 ID
        std::vector<TokenParam> InitalTokens;  // 초기 토큰
    };

    namespace ExcelData
    {
        namespace Key
        {
            namespace Data
            {
                constexpr const char8_t* SHEET_NAME     = u8"적 기본 정의";

                constexpr const char8_t* ID             = u8"ID";
                constexpr const char8_t* NAME           = u8"Name";
                constexpr const char8_t* MODEL_ID       = u8"Model ID";
                constexpr const std::array<const char8_t*, MAX_FSM_COUNT> FSM = {
                    u8"AI FSM 1", u8"AI FSM 2", u8"AI FSM 3"
                };
                constexpr const std::array<const char8_t*, MAX_SKILL_COUNT> SKILL = {
                    u8"S001", u8"S002", u8"S003", u8"S004", u8"S005"
                };
            }
            namespace Action
            {
                constexpr const char8_t* SHEET_NAME     = u8"스킬 기본 정의";
                
                constexpr const char8_t* ID             = u8"ID";
                constexpr const char8_t* NAME           = u8"Name";
                constexpr const char8_t* TYPE           = u8"Type";
                constexpr const char8_t* TARGET         = u8"Target";
                constexpr const char8_t* ATTACK_COUNT   = u8"Attack Count";
                constexpr const char8_t* PARAMETER      = u8"Parameter";
            }
            namespace Stage
            {
                constexpr const char8_t* SHEET_NAME     = u8"스테이지 별 적 조정";

                constexpr const char8_t* LEVEL_ID       = u8"Level ID";
                constexpr const char8_t* MONSTER_ID     = u8"Enemy ID";
                constexpr const char8_t* HEALTH         = u8"Health";
                constexpr const char8_t* STUN_RESIST    = u8"Stun Res";

                constexpr const std::array<const char8_t*, MAX_SKILL_COUNT> ACTION_PARAM = {
                    u8"S001_Param", u8"S002_Param", u8"S003_Param", u8"S004_Param", u8"S005_Param"
                };
                constexpr const std::array<const char8_t*, MAX_SKILL_COUNT> TOKEN_PARAM = {
                    u8"T001_Param", u8"T002_Param", u8"T003_Param", u8"T004_Param", u8"T005_Param"
                };
            }
        }
    }

    std::vector<ActionParam> ParseActionParam(const std::string& paramStr); // ex) 1, 5, 2 (데미지1, 데미지2, 데미지3)
    std::vector<TokenParam>  ParseTokenParam(const std::string& paramStr); // ex) 205003:2, 205004:1 (토큰ID:개수)
} // namespace Monster
