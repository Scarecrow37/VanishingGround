#pragma once

namespace FocusHelper
{
    enum Flag
    {
        IS_FOCUSED_NONE        = 1 << 0,
        IS_FOCUSED_INVENTORY   = 1 << 1,
        IS_FOCUSED_PREFERENCES = 1 << 2,

        IS_FOCUSED_ANITHING = IS_FOCUSED_INVENTORY | IS_FOCUSED_PREFERENCES,
    };
    using Flags = int;

    /// <summary>
    /// 인자로 넣은 플래그중 하나라도 포커스 되어있는지 확인합니다.
    /// </summary>
    bool CheckFocus(Flags flags);
}
