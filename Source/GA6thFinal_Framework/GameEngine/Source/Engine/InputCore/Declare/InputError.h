#pragma once

namespace Input
{
    using Result = unsigned char;

    constexpr Result INPUT_ERROR_SUCCESS       = 0;
    constexpr Result INPUT_ERROR_LOST_DEVICE   = 1;
    constexpr Result INPUT_ERROR_NO_CHANGE     = 2;
    constexpr Result INPUT_ERROR_WRONG_ID      = 3;
    constexpr Result INPUT_ERROR_NOT_CONNECTED = 4;
    constexpr Result INPUT_ERROR_UNKNOWN        = 255;
} // namespace Input