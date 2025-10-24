#pragma once

namespace ContentMath
{
    /// <summary>
    /// 퍼센트를 올림으로 계산합니다.
    /// </summary>
    /// <param name="baseValue :">비율을 구할 대상 숫자</param>
    /// <param name="percentage :">비율 (단위 : %)</param>
    /// <returns></returns>
    inline int CeilPercentage(int baseValue, int percentage)
    {
        if (baseValue <= 0 || percentage <= 0)
        {
            return 0;
        }
        long long product = static_cast<long long>(baseValue) * percentage;
        return static_cast<int>((product + 99) / 100);
    }
}