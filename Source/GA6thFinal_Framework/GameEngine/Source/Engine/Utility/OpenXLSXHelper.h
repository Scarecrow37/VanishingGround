#pragma once

#pragma warning(disable : 4244)
#pragma warning(disable : 4267)

#ifdef _DEBUG
#pragma comment(lib, "OpenXLSXd.lib")
#else
#pragma comment(lib, "OpenXLSX.lib")
#endif

#include "OpenXLSX/OpenXLSX.hpp"

#pragma warning(default : 4244)
#pragma warning(default : 4267)

namespace OpenXLSXHelper
{
    using Colum = unsigned int;
    using Row   = unsigned int;

    constexpr Colum FAIL_COLUM = std::numeric_limits<unsigned int>::max();
    constexpr Row   FAIL_ROW   = std::numeric_limits<unsigned int>::max();

    /// <summary>
    /// Find 함수 성공 여부를 체크합니다.
    /// </summary>
    inline bool IsFindSuccess(const std::pair<Colum, Row>& findValue)
    {
        bool result = findValue.first != FAIL_COLUM || findValue.second != FAIL_ROW;
        return result;
    }

    /// <summary>
    /// 워크시트에 해당 데이터가 존재하면 첫번째로 만난 Colum, Row 인덱스를 반환합니다.
    /// </summary>
    /// <param name="workSheet :">찾을 워크시트</param>
    /// <param name="findData :">찾을 데이터</param>
    /// <returns>실패시 unsigend int max를 반환합니다.</returns>
    std::pair<Colum, Row> FindColumRowToData(const OpenXLSX::XLWorksheet& workSheet, std::string_view findData);
}