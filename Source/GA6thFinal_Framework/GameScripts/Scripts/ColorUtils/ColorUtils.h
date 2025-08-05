#pragma once

struct ColorUtils
{
    /// <summary>
    /// 0~255 사이의 값을 0~1 사이로 변환합니다.
    /// </summary>
    /// <param name="color :">변환할 color 값</param>
    /// <returns></returns>
    inline static constexpr float ColorNormalized(unsigned char color) { return static_cast<float>(color) / 255.f; }


};