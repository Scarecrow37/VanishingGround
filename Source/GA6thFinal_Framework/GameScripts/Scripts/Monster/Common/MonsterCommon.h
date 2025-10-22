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
    constexpr size_t MAX_DIFF_COUNT  = 2;
    
    constexpr const std::array<const char*, MAX_ENEMY_COUNT> SPAWN_POINT_TAGS = {
        "Enemy Spawn Point Left",
        "Enemy Spawn Point Middle",
        "Enemy Spawn Point Right"};

    enum class SpawnPoint
    {
        Invalid = -1,
        Left    = 0,
        Middle  = 1,
        Right   = 2,
    };
    struct StatParam
    {
        int Param = 0;
    };
    struct ActionParam
    {
        int Param = 0;
    };
    struct TokenParam
    {
        int TokenID = 0; // 토큰 ID
        int Count   = 0; // 토큰 개수
    };
    
    struct SpawnParam
    {
        DataID MonsterID = 0; // 몬스터 ID
        std::vector<TokenParam> InitialTokens; // 초기 토큰
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
            namespace Stat
            {
                constexpr const std::array<const char8_t*, 2> DIFFICULTY_LIST = {u8"(일반)", u8"(어려움)"};

                constexpr const char8_t* SHEET_NAME     = u8"스테이지 별 적 조정";

                constexpr const char8_t* LEVEL_ID       = u8"Level ID";
                constexpr const char8_t* MONSTER_ID     = u8"Enemy ID";
                constexpr const char8_t* HEALTH         = u8"Health";
                constexpr const char8_t* STUN_RESIST    = u8"Stun Res";
                constexpr const char8_t* PARAM          = u8"Param";

                constexpr const std::array<const char8_t*, MAX_SKILL_COUNT> ACTION_PARAM = {
                    u8"S001_Param", u8"S002_Param", u8"S003_Param", u8"S004_Param", u8"S005_Param"
                };
                constexpr const std::array<const char8_t*, MAX_SKILL_COUNT> TOKEN_PARAM = {
                    u8"T001_Param", u8"T002_Param", u8"T003_Param", u8"T004_Param", u8"T005_Param"
                };
            }
            namespace Spawn
            {
                constexpr const char8_t* SHEET_NAME = u8"몬스터 등장 테이블";

                constexpr const char8_t* ID             = u8"ID";
                constexpr const std::array<const char8_t*, MAX_DIFF_COUNT>  LEVEL_ID = {u8"LevelID1", u8"LevelID2"};
                constexpr const std::array<const char8_t*, MAX_ENEMY_COUNT> TILE = {u8"Xtile", u8"Ytile", u8"Btile"};
                constexpr const std::array<const char8_t*, MAX_ENEMY_COUNT> BUFF = {u8"X_Buff", u8"Y_Buff", u8"B_Buff"};
                
            }
        }
    } // namespace ExcelData

    int                      StringToInt(std::string_view str);
    bool                     StringToInt(std::string_view str, int& outValue);
    std::vector<int>         ParseParam(std::string_view paramStr); // ex) 1, 5, 2 (데미지1, 데미지2, 데미지3)
    std::vector<TokenParam>  ParseTokenParam(std::string_view paramStr);  // ex) 205003:2, 205004:1 (토큰ID:개수)

    /// <summary>
    /// SpawnID를 얻어옵니다.
    /// </summary>
    /// <param name="levelID">메인 Level 인덱스입니다.</param>
    /// <param name="subLevelIndex">서브 Level 인덱스입니다.</param>
    /// <param name="battleCount">전투 횟수 카운트입니다.</param>
    constexpr const char*   SPAWN_ID_HEADER = "211";
    SpawnID                 GetSpawnID(size_t mainLevelIndex, size_t subLevelIndex, size_t battleCount);

    constexpr const char*   SpawnPointToString(SpawnPoint point)
    {
        switch (point)
        {
        case SpawnPoint::Left:
            return "Left";
        case SpawnPoint::Middle:
            return "Middle";
        case SpawnPoint::Right:
            return "Right";
        default:
            break;
        }
        return "";
    }
} // namespace Monster
