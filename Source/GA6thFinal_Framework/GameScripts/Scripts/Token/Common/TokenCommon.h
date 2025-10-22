#pragma once
using TokenID = int;

enum class TokenTag
{
    NONE,
    BLEED,
    POISON,
    STUN,
    ARMOR,
    REGEN,
    HASTE,
};

struct TokenData
{
    int                 ID;
    std::string         Name;
    int                 Order;
    int                 MaxStack;
    std::vector<int>    Params;
};

namespace TokenExcelData
{
    namespace Key
    {
        constexpr const char8_t* ID         = u8"ID";
        constexpr const char8_t* NAME       = u8"RealName";
        constexpr const char8_t* ORDER      = u8"Order";
        constexpr const char8_t* MAX_STACK  = u8"MaxStack";
        constexpr const char8_t* PARAMETER  = u8"Parameter";
    }
} // namespace ExcelData