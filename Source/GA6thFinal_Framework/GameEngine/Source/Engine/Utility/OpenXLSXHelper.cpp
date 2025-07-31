#include "pch.h"

std::pair<unsigned int, unsigned int> OpenXLSXHelper::FindColumRowToData(const OpenXLSX::XLWorksheet& workSheet,
                                                                         std::string_view             findData)
{
    unsigned int colum;
    unsigned int row;
    unsigned int columCount = workSheet.columnCount();
    unsigned int rowCount = workSheet.rowCount();
    for (colum = 1; colum <= columCount; ++colum)
    {
        for (row = 1; row <= rowCount; ++row)
        {
            auto dataValue = workSheet.cell(colum, row);
            if (dataValue)
            {
                std::string data = dataValue.getString();
                if (data == findData)
                {
                    return std::pair<unsigned int, unsigned int>(colum, row);
                }
            }
        }
    }
    colum = FAIL_COLUM;
    row   = FAIL_ROW;
    return std::pair<unsigned int, unsigned int>(colum, row);
}
