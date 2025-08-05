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

std::vector<std::pair<std::string, std::vector<std::string>>> OpenXLSXHelper::ParseSheetWithColumnKeys(const OpenXLSX::XLWorksheet& workSheet, Row keyRowIndex)
{
    std::vector<std::pair<std::string, std::vector<std::string>>> sheetDatas;

    unsigned int rowCount = workSheet.rowCount();
    unsigned int rowStart = keyRowIndex + 1;
    if (rowStart <= rowCount)
    {
        unsigned int columnCount = workSheet.columnCount();
        sheetDatas.reserve(columnCount);
        for (unsigned int column = 1; column <= columnCount; ++column)
        {
            auto keyValue = workSheet.cell(keyRowIndex, column);
            if (keyValue)
            {
                std::string key = keyValue.getString();
                if (false == key.empty())
                {
                    std::vector<std::string> datas(size_t(rowCount - rowStart + 1));
                    for (unsigned int row = rowStart; row <= rowCount; ++row)
                    {
                        auto dataValue = workSheet.cell(row, column);
                        if (dataValue)
                        {
                            unsigned int index = row - rowStart;
                            datas[index]       = dataValue.getString();
                        }
                    }
                    sheetDatas.emplace_back(key, datas);
                }
            }
        }
    }       
    return sheetDatas;
}