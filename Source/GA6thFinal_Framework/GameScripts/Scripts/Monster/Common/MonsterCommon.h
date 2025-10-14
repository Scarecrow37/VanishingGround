#pragma once

namespace Monster
{
    using FSMID         = int; // 몬스터 FSM ID
    using DataID        = int; // 몬스터 ID
    using ActionID      = int; // 몬스터 액션 ID

    constexpr size_t MAX_ENEMY_COUNT = 3;
    constexpr size_t MAX_FSM_COUNT   = 3;
    constexpr size_t MAX_SKILL_COUNT = 5;
    
    constexpr const std::array<const char*, MAX_ENEMY_COUNT> SPAWN_POINT_TAGS = {
        "Enemy Spawn Point Left",
        "Enemy Spawn Point Middle",
        "Enemy Spawn Point Right"};

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

                constexpr const char8_t* STAGE_ID       = u8"Stage ID";
                constexpr const char8_t* MONSTER_ID     = u8"Enemy ID";
                constexpr const char8_t* HEALTH         = u8"Health";
                constexpr const char8_t* STUN_RESIST    = u8"Stun Res";
            }
        }
    }
} // namespace Monster
