#include "pchScripts.h"
#include "ExcelDataSystem.h"

UMREAL_COMPONENT(ExcelDataSystem)

ExcelDataSystem::ExcelDataSystem() 
    : 
#ifdef _UMEDITOR
    _excelParser{"64F8C1F9-344D-4D36-A232-47E68DA36134", u8"ID"}, 
#endif
    _singletonObject{this}
{

}

ExcelDataSystem::~ExcelDataSystem()
{

}

std::unique_ptr<ExcelDataBase> ExcelDataSystem::GetExcelDataBase(const std::string& sheetName)
{
    std::unique_ptr<ExcelDataBase> excelDataBase;

    auto& dataBase = ReflectFields->DataBase;
    auto  findIter = dataBase.find(sheetName);
    if (findIter != dataBase.end())
    {
        excelDataBase.reset(new ExcelDataBase(findIter->second));
    }
    return excelDataBase;
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
        ExcelSystemMapType& dataBase = ReflectFields->DataBase;
        ExcelDataBase::DataBaseType& dataSheetTuple = dataBase[selectSheetName];
        auto& [columnIndexKeyMap, keyIndexToMap, dataSheet] = dataSheetTuple;
        columnIndexKeyMap.clear();
        keyIndexToMap.clear();
        dataSheet.clear();
        size_t currRowIndex = 0;
        _excelParser.Apply([&](const ImGuiColumnSheetParser::ColumnDatas& datas) 
        {   
            size_t currColumnIndex = 0;
            for (auto& pair : datas)
            {
                const std::string& key = pair.first;
                const std::string& data = pair.second;
                if (false == key.empty())
                {
                    auto& columnAndrow = keyIndexToMap[key];
                    auto& [columnIndex, rowIndexMap] = columnAndrow;
                    columnIndex = currColumnIndex;                       // Key에 대한 Column 인덱스 등록
                    if (columnIndexKeyMap.size() <= currColumnIndex)
                    {
                        columnIndexKeyMap.emplace_back(key);             // Column 인덱스에 대한 Key 등록
                    }
                    rowIndexMap.try_emplace(data, currRowIndex);         // Data에 대한 Row 인덱스 등록

                    //2차원 배열에 데이터 등록
                    if (dataSheet.size() <= currRowIndex)
                    {
                        dataSheet.resize(currRowIndex + 1);
                    }
                    dataSheet[currRowIndex].push_back(data);
                }
                ++currColumnIndex;
            }
            ++currRowIndex;
        });
        _excelParser.ShowParser = false;
    }
#endif 
}

void ExcelDataSystem::ImGuiDrawDataSheetView() 
{
#ifdef _UMEDITOR
    if (ImGui::Button("Data Base Viewer"))
    {
        _drawDataBaseView = true;
    }

    ImGui::Begin("Data Base Viewer##64F8C1F9-344D-4D36-A232-47E68DA36134", &_drawDataBaseView);
    {
        if (ImGui::TreeNode("Sheet Viewer"))
        {
            static thread_local std::string viewerSheetName = STR_NULL;
            auto&                           dataBase        = ReflectFields->DataBase;
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
                auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = findIter->second;
                if (false == columnIndexKeyMap.empty())
                {
                    int columnCount = static_cast<int>(columnIndexKeyMap.size());
                    if (ImGui::BeginTable("##SheetViewerTable", columnCount,
                                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                    {
                        for (auto& key : columnIndexKeyMap)
                        {
                            ImGui::TableSetupColumn(key.c_str());
                        }
                        ImGui::TableHeadersRow();

                        for (auto& raw : dataSheet)
                        {
                            int columnIndex = 0;
                            for (auto& column : raw)
                            {
                                ImGui::TableSetColumnIndex(columnIndex);
                                ImGui::Text(column.c_str());
                                ++columnIndex;
                            }
                            ImGui::TableNextRow();
                        }
                        ImGui::EndTable();
                    }
                }
            }
            ImGui::TreePop();
        }    
    }
    ImGui::End();
#endif
}

ExcelDataBase::ExcelDataBase(const ExcelDataBase& rhs) 
    :
    _dataBase(rhs._dataBase) 
{

}

size_t ExcelDataBase::FindColumnIndex(const std::string& columnKeyName)
{
    size_t result = FIND_INDEX_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) //댕글링 방지
    {
        auto& [columnIndexKeyMap, indexData, dataSheet] = _dataBase;
        auto findIter = indexData.find(columnKeyName);
        if (findIter != indexData.end())
        {
            auto& [columnIndex, rowIndexMap] = findIter->second;
            result = columnIndex;
        }
    }
    return result;
}

std::string_view ExcelDataBase::FindColumnKey(size_t columnIndex)
{
    std::string_view result = FIND_STR_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        auto& [columnIndexKeyMap, indexData, dataSheet] = _dataBase;
        if (columnIndex < columnIndexKeyMap.size())
        {
            result = columnIndexKeyMap[columnIndex];
        }
    }
    return result;
}

std::string_view ExcelDataBase::FindData(size_t rowIndex, size_t columnIndex)
{
    std::string_view result = FIND_STR_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        auto& [columnIndexKeyMap, indexData, dataSheet] = _dataBase;
        if (rowIndex < dataSheet.size())
        {
            auto& columnDatas = dataSheet[rowIndex];
            if (columnIndex < columnDatas.size())
            {
                result = columnDatas[columnIndex];
            }
        }
    }
    return result;
}

ExcelDataBase::ExcelDataBase(const DataBaseType& dataBase) 
    : 
    _dataBase(dataBase) 
{

}

ExcelDataBase::~ExcelDataBase() 
{

}
