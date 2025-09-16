#include "pchScripts.h"
#include "ExcelDataSystem.h"
ExcelDataSystem::ExcelDataSystem() 
    : 
    _excelParser{"64F8C1F9-344D-4D36-A232-47E68DA36134", u8"ID"} 
{

}

ExcelDataSystem::~ExcelDataSystem()
{

}

void ExcelDataSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Excel parser"))
    {
        _excelParser.ShowParser = true;
    }
    ImGuiDrawExcelParserEdit();
}

void ExcelDataSystem::ImGuiDrawExcelParserEdit()
{
#ifdef _UMEDITOR
    if (_excelParser.Draw())
    {
        const std::string& selectSheetName = _excelParser.GetSelectSheetName();
        size_t rawCount    = _excelParser.GetRawCount();
        size_t columnCount = _excelParser.GetColumnCount();
        DataBaseType& dataBase = ReflectFields->DataBase;
        std::pair<KeyIndexMapType, DataSheetType>& dataSheetPair = dataBase[selectSheetName];
        auto& [keyIndexToMap, dataSheet] = dataSheetPair;
        keyIndexToMap.clear();
        dataSheet.clear();
        size_t dataIndex = 0;
        _excelParser.Apply([&](const ImGuiColumnSheetParser::ColumnDatas& datas) 
        { 
            bool isEmptyColumn = true;
            for (auto& pair : datas)
            {
                const std::string& key = pair.first;
                const std::string& data = pair.second;
                if (false == key.empty())
                {
                    isEmptyColumn  = data.empty();
                    auto& indexMap = keyIndexToMap[key];
                    indexMap.try_emplace(data, dataIndex);
                    if (dataSheet.size() <= dataIndex)
                    {
                        dataSheet.resize(dataIndex + 1);
                    }
                    dataSheet[dataIndex].push_back(data);
                }
            }
            if (isEmptyColumn)
            {

            }
            ++dataIndex;
        });
        _excelParser.ShowParser = true;
    }
#endif 
}
