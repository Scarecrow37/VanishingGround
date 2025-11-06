#pragma once

namespace Tooltip
{
    struct TooltipData
    {
        int         ImageAssetId;
        std::string Title;
        std::string Description;
    };

    enum class Group
    {
        PLAYER,
        ENEMY,
        REVELATION
    };
}
