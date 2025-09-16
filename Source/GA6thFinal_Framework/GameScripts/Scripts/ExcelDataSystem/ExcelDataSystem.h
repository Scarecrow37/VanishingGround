#pragma once
#include "DLLExportDefine.h"
#include "ExcelParser/ImGuiColumnSheetParser.h"
#include "Utility/SingletonHelper.h"

class ExcelDataSystem : public Component
{
    USING_PROPERTY(ExcelDataSystem)

public:
    ExcelDataSystem();
    ~ExcelDataSystem() override;

private:
    SingletonObject<ExcelDataSystem> _singletonObject;

public:
    REFLECT_PROPERTY()

protected:
    using KeyIndexMapType = std::unordered_map<std::string, std::unordered_map<std::string, size_t>>;
    using DataSheetType   = std::vector<std::vector<std::string>>;
    using DataBaseType    = std::unordered_map<std::string, std::pair<KeyIndexMapType, DataSheetType>>;
    REFLECT_FIELDS_BEGIN(Component)
    DataBaseType DataBase;
    REFLECT_FIELDS_END(ExcelDataSystem)

    void Reset() override;
    void Awake() override;
    void ImGuiDrawPropertysEvent() override;

private:
#ifdef _UMEDITOR
    ImGuiColumnSheetParser _excelParser;
#endif
    void ImGuiDrawExcelParserEdit();
    void ImGuiDrawDataSheetView();

};
