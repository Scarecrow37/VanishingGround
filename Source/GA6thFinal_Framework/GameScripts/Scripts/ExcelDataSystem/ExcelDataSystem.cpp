#include "pchScripts.h"
#include "ExcelDataSystem.h"

UMREAL_COMPONENT(ExcelDataSystem)

using namespace u8_literals;

ExcelDataSystem::ExcelDataSystem() 
    : 
#ifdef _UMEDITOR
    _excelParser{"64F8C1F9-344D-4D36-A232-47E68DA36134", u8"ID"}, 
#endif
    _singletonObject{this},
    _singletonComponent{this}
{

}

ExcelDataSystem::~ExcelDataSystem()
{

}

std::unique_ptr<ExcelDataBase> ExcelDataSystem::FindExcelDataBase(std::u8string_view sheetName)
{
    std::unique_ptr<ExcelDataBase> excelDataBase;

    auto& dataBase = ReflectFields->DataBase;
    auto  findIter = dataBase.find((const char*)sheetName.data());
    if (findIter != dataBase.end())
    {
        excelDataBase.reset(new ExcelDataBase(findIter->first));
    }
    return excelDataBase;
}

const ExcelDataBase::DataBaseType* ExcelDataSystem::GetRowDataBase(std::u8string_view sheetName)
{
    ExcelDataBase::DataBaseType* result = nullptr;

    auto& dataBase = ReflectFields->DataBase;
    auto  findIter = dataBase.find((const char*)sheetName.data());
    if (findIter != dataBase.end())
    {
        result = &findIter->second;
    }
    return result;
}

void ExcelDataSystem::Reset() 
{
    _singletonObject.SetSingleTon();
    _singletonComponent.SetSingleTon();
}

void ExcelDataSystem::Added() 
{
    _singletonObject.TrySingleTon(true);
    _singletonComponent.TrySingleTon();
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
        _drawDataBaseView       = true;
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

    if (ImGui::TreeNode("Sheet Data List"))
    {
        for (auto& [key, dataBase] : ReflectFields->DataBase)
        {
            ImGui::Text(key.c_str());
        }
        ImGui::TreePop();
    }

    if (_drawDataBaseView)
    {
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
                ImGui::SameLine();
                if (ImGui::Button("Delete") && viewerSheetName != STR_NULL)
                {
                    if (0 < dataBase.erase(viewerSheetName))
                    {
                        viewerSheetName = STR_NULL;
                    }
                }

                if (auto findIter = dataBase.find(viewerSheetName); findIter != dataBase.end())
                {
                    auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = findIter->second;
                    if (false == columnIndexKeyMap.empty())
                    {
                        ImGuiDrawFindTest(viewerSheetName);                     
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
                    else
                    {
                        ImGui::Text((const char*)u8"빈 데이터 시트입니다.");
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
#endif
}

void ExcelDataSystem::ImGuiDrawFindTest(const std::string& viewerSheetNameStr)
{
    if (ImGui::TreeNode("Find Test"))
    {
        std::u8string_view viewerSheetName = (const char8_t*)viewerSheetNameStr.data();
        if (ImGui::TreeNode("Column Index"))
        {
            static std::string buffer;
            static size_t      result = ExcelDataBase::FIND_INDEX_FAIL;
            ImGui::InputText("##Column Index", &buffer);
            ImGui::SameLine();
            if (ImGui::Button("Find Column Index"))
            {
                std::unique_ptr<ExcelDataBase> dataBase = FindExcelDataBase(viewerSheetName);
                result = dataBase->FindColumnIndex((const char8_t*)buffer.c_str());
            }
            ImGui::Text((const char*)u8"결과 :");
            if (result != ExcelDataBase::FIND_INDEX_FAIL)
            {
                ImGui::SameLine();
                ImGui::Text(std::to_string(result).c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Column Key"))
        {
            static int         buffer;
            static std::string result = ExcelDataBase::FIND_STR_FAIL.data();
            ImGui::InputInt("##Column Key", &buffer);
            ImGui::SameLine();
            if (ImGui::Button("Find Column Key"))
            {
                std::unique_ptr<ExcelDataBase> dataBase = FindExcelDataBase(viewerSheetName);
                result = dataBase->FindColumnKey((size_t)buffer);
            }
            ImGui::Text((const char*)u8"결과 :");
            if (result != ExcelDataBase::FIND_STR_FAIL)
            {
                ImGui::SameLine();
                ImGui::Text(result.c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Row Index"))
        {
            static char   rowKeyBuffer[128]    = "";
            static char   columnKeyBuffer[128] = "";
            static size_t result               = ExcelDataBase::FIND_INDEX_FAIL;
            ImGui::InputText("Row Key##RowIndex", rowKeyBuffer, sizeof(rowKeyBuffer));
            ImGui::InputText("Column Key##RowIndex", columnKeyBuffer, sizeof(columnKeyBuffer));
            if (ImGui::Button("Find Row Index"))
            {
                std::unique_ptr<ExcelDataBase> dataBase = FindExcelDataBase(viewerSheetName);
                result = dataBase->FindRowIndex((const char8_t*)rowKeyBuffer, (const char8_t*)columnKeyBuffer);
            }
            ImGui::Text((const char*)u8"결과 :");
            if (result != ExcelDataBase::FIND_INDEX_FAIL)
            {
                ImGui::SameLine();
                ImGui::Text(std::to_string(result).c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Data"))
        {
            static int         rowIndexBuffer       = 0;
            static char        columnKeyBuffer[128] = "";
            static std::string result               = ExcelDataBase::FIND_STR_FAIL.data();
            ImGui::InputInt("Row Index##Data", &rowIndexBuffer);
            ImGui::InputText("Column Key##Data", columnKeyBuffer, sizeof(columnKeyBuffer));
            if (ImGui::Button("Find Data"))
            {
                std::unique_ptr<ExcelDataBase> dataBase = FindExcelDataBase(viewerSheetName);
                result = dataBase->FindData((size_t)rowIndexBuffer, (const char8_t*)columnKeyBuffer);
            }
            ImGui::Text((const char*)u8"결과 :");
            if (result != ExcelDataBase::FIND_STR_FAIL)
            {
                ImGui::SameLine();
                ImGui::Text(result.c_str());
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("GetColumnDatas Test"))
        {
            if (ImGui::TreeNode("By Row Index"))
            {
                static int                      rowIndex = 0;
                static std::vector<std::string> result_datas;
                static bool                     found = false;

                ImGui::InputInt("Row Index##GetColumnDatas", &rowIndex);
                ImGui::SameLine();
                if (ImGui::Button("Get##GetColumnDatas_RowIndex"))
                {
                    std::unique_ptr<ExcelDataBase>  dataBase = FindExcelDataBase(viewerSheetName);
                    const std::vector<std::string>* dataPtr  = dataBase->GetColumnDatas((size_t)rowIndex);
                    if (dataPtr)
                    {
                        result_datas = *dataPtr; // 데이터를 복사하여 안전하게 표시
                        found        = true;
                    }
                    else
                    {
                        found = false;
                    }
                }

                if (found)
                {
                    ImGui::Text("Found %zu columns of data:", result_datas.size());
                    if (ImGui::BeginChild("DataPreview##RowIndex", ImVec2(0, 100), true))
                    {
                        for (size_t i = 0; i < result_datas.size(); ++i)
                        {
                            ImGui::Text("[%zu]: %s", i, result_datas[i].c_str());
                        }
                    }
                    ImGui::EndChild();
                }
                else
                {
                    ImGui::Text((const char*)u8"결과: 데이터를 찾을 수 없습니다.");
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("By RowKey and ColumnKey"))
            {
                static char                     rowKey[128] = "";
                static char                     colKey[128] = "";
                static std::vector<std::string> result_datas;
                static bool                     found = false;

                ImGui::InputText("Row Key##GetColumnDatas", rowKey, sizeof(rowKey));
                ImGui::InputText("Column Key##GetColumnDatas", colKey, sizeof(colKey));

                if (ImGui::Button("Get##GetColumnDatas_Keys"))
                {
                    std::unique_ptr<ExcelDataBase>  dataBase = FindExcelDataBase(viewerSheetName);
                    const std::vector<std::string>* dataPtr  = dataBase->GetColumnDatas((const char8_t*)rowKey, (const char8_t*)colKey);
                    if (dataPtr)
                    {
                        result_datas = *dataPtr;
                        found        = true;
                    }
                    else
                    {
                        found = false;
                    }
                }

                if (found)
                {
                    ImGui::Text("Found %zu columns of data:", result_datas.size());
                    if (ImGui::BeginChild("DataPreview##Keys", ImVec2(0, 100), true))
                    {
                        for (size_t i = 0; i < result_datas.size(); ++i)
                        {
                            ImGui::Text("[%zu]: %s", i, result_datas[i].c_str());
                        }
                    }
                    ImGui::EndChild();
                }
                else
                {
                    ImGui::Text((const char*)u8"결과: 데이터를 찾을 수 없습니다.");
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

ExcelDataBase::ExcelDataBase(const ExcelDataBase& rhs) 
    :
    _key(rhs._key) 
{

}

size_t ExcelDataBase::FindColumnIndex(std::u8string_view columnKeyName)
{
    size_t result = FIND_INDEX_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) //댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            auto findIter = keyIndexMap.find((const char*)columnKeyName.data());
            if (findIter != keyIndexMap.end())
            {
                auto& [columnIndex, rowIndexMap] = findIter->second;
                result                           = columnIndex;
            }
        }
    }
    return result;
}

std::string_view ExcelDataBase::FindColumnKey(size_t columnIndex)
{
    std::string_view result = FIND_STR_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            if (columnIndex < columnIndexKeyMap.size())
            {
                result = columnIndexKeyMap[columnIndex];
            }
        }
    }
    return result;
}

size_t ExcelDataBase::FindRowIndex(std::u8string_view rowKey, size_t columnIndex)
{
    size_t result = FIND_INDEX_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            if (columnIndex < columnIndexKeyMap.size())
            {
                const std::string& columnKey  = columnIndexKeyMap[columnIndex];
                auto               columnIter = keyIndexMap.find(columnKey);
                if (columnIter != keyIndexMap.end())
                {
                    auto& [columnIndex, rowIndexMap] = columnIter->second;
                    auto rowIter                     = rowIndexMap.find((const char*)rowKey.data());
                    if (rowIter != rowIndexMap.end())
                    {
                        result = rowIter->second;
                    }
                }
            }
        }
    }
    return result;
}

size_t ExcelDataBase::FindRowIndex(std::u8string_view rowKey, std::u8string_view columnKey)
{
    size_t result = FIND_INDEX_FAIL;
    size_t columnIndex = FindColumnIndex(columnKey);
    if (columnIndex != FIND_INDEX_FAIL)
    {
        result = FindRowIndex(rowKey, columnIndex);
    }
    return result;
}

std::string_view ExcelDataBase::FindData(size_t rowIndex, size_t columnIndex)
{
    std::string_view result = FIND_STR_FAIL;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            if (rowIndex < dataSheet.size())
            {
                auto& columnDatas = dataSheet[rowIndex];
                if (columnIndex < columnDatas.size())
                {
                    result = columnDatas[columnIndex];
                }
            }
        }
    }
    return result;
}

std::string_view ExcelDataBase::FindData(size_t rowIndex, std::u8string_view columnKey)
{
    std::string_view result = FIND_STR_FAIL;
    size_t columnIndex = FindColumnIndex(columnKey);
    if (columnIndex != FIND_INDEX_FAIL)
    {
        result = FindData(rowIndex, columnIndex);
    }
    return result;
}

const std::vector<std::string>* ExcelDataBase::GetColumnDatas(size_t rowIndex)
{
    const std::vector<std::string>* result = nullptr;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            if (rowIndex < dataSheet.size())
            {
                result = &dataSheet[rowIndex];
            }
        }
    }
    return result;
}

const std::vector<std::string>* ExcelDataBase::GetColumnDatas(std::u8string_view rowKey, size_t columnIndex)
{
    const std::vector<std::string>* result = nullptr;
    size_t rowIndex = FindRowIndex(rowKey, columnIndex);
    if (rowIndex != FIND_INDEX_FAIL)
    {
        result = GetColumnDatas(rowIndex);
    } 
    return result;
}

const std::vector<std::string>* ExcelDataBase::GetColumnDatas(std::u8string_view rowKey, std::u8string_view columnKey)
{
    const std::vector<std::string>* result = nullptr;
    size_t columnIndex = FindColumnIndex(columnKey);
    if (columnIndex != FIND_INDEX_FAIL)
    {
        result = GetColumnDatas(rowKey, columnIndex);
    }
    return result;
}

size_t ExcelDataBase::RowCount() const
{
    size_t result = 0;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            result = dataSheet.size();
        }
    }
    return result;
}

size_t ExcelDataBase::ColumnCount()
{
    size_t result = 0;
    if (ExcelDataSystem* system = SingletonComponent<ExcelDataSystem>::GetInstance()) // 댕글링 방지
    {
        if (const DataBaseType* dataBase = system->GetRowDataBase(_key))
        {
            auto& [columnIndexKeyMap, keyIndexMap, dataSheet] = *dataBase;
            if (false == dataSheet.empty())
            {
                result = dataSheet.front().size();
            }
        }
    }
    return result;
}

ExcelDataBase::ExcelDataBase(const std::string& dataBaseKey) 
    : 
    _key((const char8_t*)dataBaseKey.data()) 
{

}

ExcelDataBase::~ExcelDataBase() 
{

}
