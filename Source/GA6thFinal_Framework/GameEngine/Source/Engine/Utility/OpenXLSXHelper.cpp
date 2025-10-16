#include "pch.h"

using namespace OpenXLSXHelper;

std::pair<Row, Column> OpenXLSXHelper::FindRowColumnToData(const OpenXLSX::XLWorksheet& workSheet, std::string_view findData)
{
    unsigned int row;
    unsigned int rowCount = workSheet.rowCount();
    unsigned int column;
    unsigned int columnCount = workSheet.columnCount();

    int emptyCount = 0;
    for (row = 1; row <= rowCount; ++row)
    {
        constexpr int emptyCountMax = 3;
        bool isEmpty = true;
        for (column = 1; column <= columnCount; ++column)
        {
            auto dataValue = workSheet.cell(row, column);
            if (dataValue)
            {
                std::string data = dataValue.getString();
                if (false == data.empty())
                {
                    if (data == findData)
                    {
                        return std::pair<unsigned int, unsigned int>(row, column);
                    }
                    isEmpty = false;
                }
            }
        }

        if (isEmpty)
        {
            if (emptyCountMax <= ++emptyCount)
            {
                break;
            }
        }
    }
    row    = FAIL_ROW;
    column = FAIL_COLUMN;
    return std::pair<unsigned int, unsigned int>(row, column);
}

std::vector<std::pair<std::string, std::vector<std::string>>> OpenXLSXHelper::ParseSheetWithColumnKeys(const OpenXLSX::XLWorksheet& workSheet, Row keyRowIndex, unsigned int threshold)
{
    std::vector<std::pair<std::string, std::vector<std::string>>> sheetDatas;

    unsigned int rowCount = GetValidRowCount(workSheet, threshold);
    unsigned int rowStart = keyRowIndex + 1;
    if (rowStart <= rowCount)
    {
        unsigned int columnCount = GetValidColumnCount(workSheet, threshold);
        sheetDatas.reserve(columnCount);
        for (unsigned int column = 1; column <= columnCount; ++column)
        {
            int  emptyRowCount = 0;
            auto keyValue = workSheet.cell(keyRowIndex, column);
            if (keyValue)
            {
                std::string key = keyValue.getString();
                if (false == key.empty())
                {
                    std::vector<std::string> datas(size_t(rowCount - rowStart + 1));
                    for (unsigned int row = rowStart; row <= rowCount; ++row)
                    {
                        constexpr int emptyRowCountMax = 1000;
                        auto dataValue = workSheet.cell(row, column);
                        if (dataValue)
                        {
                            unsigned int index = row - rowStart;
                            std::string  data  = dataValue.getString();
                            datas[index]       = std::move(data);

                            // 빈 데이터가 연속될시 해당 Column 스킵.
                            if (true == datas[index].empty())
                            {
                                if (emptyRowCountMax <= ++emptyRowCount)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    sheetDatas.emplace_back(key, datas);
                }
            }
        }
    }       
    return sheetDatas;
}

Row OpenXLSXHelper::GetValidRowCount(const OpenXLSX::XLWorksheet& workSheet, unsigned int threshold)
{
    unsigned int row;
    unsigned int rowCount = workSheet.rowCount();
    unsigned int column;
    unsigned int columnCount = workSheet.columnCount();

    unsigned int emptyRowCount = 0;
    for (row = 1; row <= rowCount; ++row)
    {
        bool isEmpty = true;
        unsigned int emptyColumnCount = 0;
        for (column = 1; column <= columnCount; ++column)
        {
            auto dataValue = workSheet.cell(row, column);
            if (dataValue)
            {
                std::string data = dataValue.getString();
                if (false == data.empty())
                {
                    isEmpty = false;
                }
                else
                {
                    if (threshold < ++emptyColumnCount)
                    {
                        break;
                    }
                }
            }
        }

        if (isEmpty)
        {
            if (threshold < ++emptyRowCount)
            {
                return row;
            }
        }
        else
        {
            emptyRowCount = 0;
        }
    }
    return rowCount;
}

Column OpenXLSXHelper::GetValidColumnCount(const OpenXLSX::XLWorksheet& workSheet, unsigned int threshold)
{
    unsigned int row;
    unsigned int rowCount = workSheet.rowCount();
    unsigned int column;
    unsigned int columnCount = workSheet.columnCount();

    unsigned int emptyColumnCount = 0;
    for (column = 1; column <= columnCount; ++column)
    {
        bool isEmpty = true;
        unsigned int emptyRowCount = 0;
        for (row = 1; row <= rowCount; ++row)
        {
            auto dataValue = workSheet.cell(row, column);
            if (dataValue)
            {
                std::string data = dataValue.getString();
                if (false == data.empty())
                {
                    isEmpty = false;
                }
                else
                {
                    if (threshold < ++emptyRowCount)
                    {
                        break;
                    }
                }
            }
        }

        if (isEmpty)
        {
            if (threshold < ++emptyColumnCount)
            {
                return column;
            }
        }
    }
    return columnCount;
}
