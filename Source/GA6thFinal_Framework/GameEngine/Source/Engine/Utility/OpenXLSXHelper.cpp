#include "pch.h"

std::pair<unsigned int, unsigned int> OpenXLSXHelper::FindRowColumnToData(const OpenXLSX::XLWorksheet& workSheet,
                                                                         std::string_view             findData)
{
    unsigned int row;
    unsigned int column;
    unsigned int columnCount = workSheet.columnCount();
    unsigned int rowCount = workSheet.rowCount();
    for (column = 1; column <= columnCount; ++column)
    {
        for (row = 1; row <= rowCount; ++row)
        {
            auto dataValue = workSheet.cell(row, column);
            if (dataValue)
            {
                std::string data = dataValue.getString();
                if (data == findData)
                {
                    return std::pair<unsigned int, unsigned int>(row, column);
                }
            }
        }
    }
    row   = FAIL_ROW;
    column = FAIL_COLUMN;
    return std::pair<unsigned int, unsigned int>(row, column);
}
