#pragma once

namespace Tooltip
{
    struct TooltipData
    {
        int         ImageAssetId;
        std::string Title;
        Color       TitleColor;
        std::string Description;
    };

    enum class Group
    {
        PLAYER,
        ENEMY,
        REVELATION
    };
}
