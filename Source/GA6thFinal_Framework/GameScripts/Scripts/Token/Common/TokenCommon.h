#pragma once
using TokenID = int;

class Player;
class Enemy;
struct PlayerStats;
struct EnemyStats;
struct WeaponStats;
namespace QTE
{
    struct NoteResult;
}

struct PlayerAttackData
{
    Player&             Source;
    PlayerStats&        SourceStats;
    WeaponStats&        WeaponStats;
    QTE::NoteResult&    NoteResult;
};

struct EnemyAttackData
{
    Enemy&              Source;
    EnemyStats&         SourceStats;
};

struct PlayerHitData
{
    Player&      Source;
    PlayerStats& SourceStats;
};

struct EnemyHitData
{
    Enemy&      Source;
    EnemyStats& SourceStats;
};

struct TokenData
{
    int                 ID = 0;
    std::string         Name;
    std::string         Tag;
    int                 Order = 50;
    int                 MaxStack = 999;
    std::vector<int>    Params;
};

namespace TokenExcelData
{
    namespace Key
    {
        constexpr const char8_t* ID         = u8"ID";
        constexpr const char8_t* NAME       = u8"RealName";
        constexpr const char8_t* TAG        = u8"Tag";
        constexpr const char8_t* ORDER      = u8"Order";
        constexpr const char8_t* MAX_STACK  = u8"MaxStack";
        constexpr const char8_t* PARAMETER  = u8"Parameter";
    }
} // namespace ExcelData