#include "pchScripts.h"
#include "ExcelDataSystem.h"

UMREAL_COMPONENT(ExcelDataSystem)

ExcelDataSystem::ExcelDataSystem() 
    : 
    _excelParser{"64F8C1F9-344D-4D36-A232-47E68DA36134", u8"ID"}, 
    _singletonObject{this}
{

}

ExcelDataSystem::~ExcelDataSystem()
{

}

void ExcelDataSystem::Reset() 
{
    _singletonObject.SetSingleTon();
}

void ExcelDataSystem::Awake() 
{
    _singletonObject.TrySingleTon(true);
}

void ExcelDataSystem::ImGuiDrawPropertysEvent()
{
    ImGuiDrawExcelParserEdit();
    ImGuiDrawDataSheetView();
}

void ExcelDataSystem::ImGuiDrawExcelParserEdit()
{
#ifdef _UMEDITOR
    if (ImGui::Button("Excel parser"))
    {
        _excelParser.ShowParser = true;
    }

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
            for (auto& pair : datas)
            {
                const std::string& key = pair.first;
                const std::string& data = pair.second;
                if (false == key.empty())
                {
                    auto& indexMap = keyIndexToMap[key];
                    indexMap.try_emplace(data, dataIndex);
                    if (dataSheet.size() <= dataIndex)
                    {
                        dataSheet.resize(dataIndex + 1);
                    }
                    dataSheet[dataIndex].push_back(data);
                }
            }
            ++dataIndex;
        });
        _excelParser.ShowParser = false;
    }
#endif 
}

void ExcelDataSystem::ImGuiDrawDataSheetView() 
{
#ifdef _UMEDITOR
    if (ImGui::TreeNode("Sheet Viewer"))
    {
        static thread_local std::string viewerSheetName = STR_NULL;
        auto& dataBase = ReflectFields->DataBase;
        if (ImGui::BeginCombo("Select sheet", viewerSheetName.c_str()))
        {
            for (auto& [key, dataPair] : dataBase)
            {
                if (ImGui::Selectable(key.c_str(), key == viewerSheetName))
                {
                    viewerSheetName = key;
                }
            }
            ImGui::EndCombo();
        }
        if (auto findIter = dataBase.find(viewerSheetName); findIter != dataBase.end())
        {
            auto& [keyIndexMap, dataSheet] = findIter->second;
            if (false == dataSheet.empty())
            {
                int columnCount = static_cast<int>(dataSheet.front().size());
                if (ImGui::BeginTable("##SheetViewerTable", columnCount, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    for (auto& raw : dataSheet)
                    {
                        ImGui::TableNextRow();
                        int columnIndex = 0;
                        for (auto& column : raw)
                        {
                            ImGui::TableSetColumnIndex(columnIndex);
                            ImGui::Text(column.c_str());
                            ++columnIndex;
                        }
                    }
                    ImGui::EndTable();
                }
            }
        }
        ImGui::TreePop();
    }    
#endif
}
