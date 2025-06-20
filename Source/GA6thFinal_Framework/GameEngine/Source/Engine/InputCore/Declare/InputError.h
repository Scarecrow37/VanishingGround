#pragma once

namespace Input
{
    using Result = unsigned char;

    constexpr Result ERROR_SUCCESS       = 0;
    constexpr Result ERROR_LOST_DEVICE   = 1;
    constexpr Result ERROR_NO_CHANGE     = 2;
    constexpr Result ERROR_WRONG_ID      = 3;
    constexpr Result ERROR_NOT_CONNECTED = 4;
    constexpr Result ERROR_UNKOWN        = 255;
} // namespace Input