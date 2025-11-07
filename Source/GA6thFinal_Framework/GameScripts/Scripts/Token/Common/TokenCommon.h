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
    int                 ID          = 0;
    int                 IconID      = 0;
    int                 InfoID      = 0;
    int                 ImageID     = 0;
    Color               NameColor;
    std::string         Name;
    std::string         Tag;
    int                 Order       = 50;
    int                 MaxStack    = 999;
    std::vector<int>    Params;
};

namespace TokenExcelData
{
    namespace Key
    {
        constexpr const char8_t* ID         = u8"ID";
        constexpr const char8_t* ICON_ID    = u8"Icon ID";
        constexpr const char8_t* INFO_ID    = u8"Info ID";
        constexpr const char8_t* IMAGE_ID   = u8"ImageID";
        constexpr const char8_t* NAME_COLOR = u8"Color";
        constexpr const char8_t* NAME       = u8"RealName";
        constexpr const char8_t* TAG        = u8"Tag";
        constexpr const char8_t* ORDER      = u8"Order";
        constexpr const char8_t* MAX_STACK  = u8"MaxStack";
        constexpr const char8_t* PARAMETER  = u8"Parameter";
    }
} // namespace ExcelData