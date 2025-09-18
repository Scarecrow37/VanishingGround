#pragma once

#include "ExcelParser/ImGuiColumnSheetParser.h"
#include "Utility/SingletonHelper.h"

class ExcelDataBase
{
public:
    //인덱스 찾기 실패시 반환되는 값
    inline static constexpr size_t FIND_INDEX_FAIL = std::numeric_limits<size_t>::max();    

    //string 찾기 실패시 반환되는 값
    inline static constexpr std::string_view FIND_STR_FAIL = STR_NULL;

    using ColumnIndexKeyMap = std::vector<std::string>;
    using KeyIndexMapType   = std::unordered_map<std::string, std::pair<size_t, std::unordered_map<std::string, size_t>>>;
    using DataSheetType     = std::vector<std::vector<std::string>>;
    using DataBaseType      = std::tuple<ColumnIndexKeyMap, KeyIndexMapType, DataSheetType>;

    ExcelDataBase(const ExcelDataBase& rhs);
    ~ExcelDataBase();
    ExcelDataBase& operator=(const ExcelDataBase& rhs) = delete;

public:
    /// <summary>
    /// 해당 문자열 키에 해당하는 Column 인덱스를 반환합니다.
    /// </summary>
    /// <param name="columnKeyName :">찾을 Column의 키 문자열</param>
    /// <returns>실패시 ExcelDataBase::FIND_INDEX_FAIL를 반환합니다.</returns>
    size_t FindColumnIndex(const std::string& columnKeyName);

    /// <summary>
    /// 해당 인덱스에 해당하는 Column key를 반환합니다.
    /// </summary>
    /// <param name="columnIndex :">찾을 columnIndex</param>
    /// <returns>실패시 ExcelDataBase::FIND_STR_FAIL을 반환합니다.</returns>
    std::string_view FindColumnKey(size_t columnIndex);

    /// <summary>
    /// 해당 columnIndex에 Row key의 인덱스를 반환합니다.
    /// </summary>
    /// <param name="rowKey :">찾을 문자열 데이터</param>
    /// <param name="columnIndex :">탐색할 column index</param>
    /// <returns>실패시 ExcelDataBase::FIND_INDEX_FAIL</returns>
    size_t FindRowIndex(const std::string& rowKey, size_t columnIndex);

    /// <summary>
    /// 해당 column key에 row key의 인덱스를 반환합니다.
    /// </summary>
    /// <param name="rowKey :">찾을 문자열 데이터</param>
    /// <param name="columnIndex :">탐색할 column 키 index</param>
    /// <returns>실패시 ExcelDataBase::FIND_INDEX_FAIL</returns>
    size_t FindRowIndex(const std::string& rowKey, const std::string& columnKey);

    /// <summary>
    /// 데이터 시트에서 데이터를 가져옵니다.
    /// </summary>
    /// <param name="rowIndex :">row Index</param>
    /// <param name="columnIndex :">column Index</param>
    /// <returns>실패시 FIND_STR_FAIL을 반환합니다.</returns>
    std::string_view FindData(size_t rowIndex, size_t columnIndex);

    /// <summary>
    /// 데이터 시트에서 데이터를 가져옵니다.
    /// </summary>
    /// <param name="rowKey :">row Key</param>
    /// <param name="columnIndex :">column Index</param>
    /// <returns>실패시 FIND_STR_FAIL을 반환합니다.</returns>
    std::string_view FindData(const std::string& rowKey, size_t columnIndex);

    /// <summary>
    /// 데이터 시트에서 데이터를 가져옵니다.
    /// </summary>
    /// <param name="rowIndex :">row index</param>
    /// <param name="columnKey :">column key</param>
    /// <returns>실패시 FIND_STR_FAIL을 반환합니다.</returns>
    std::string_view FindData(size_t rowIndex, const std::string& columnKey);

    /// <summary>
    /// 데이터 시트에서 데이터를 가져옵니다.
    /// </summary>
    /// <param name="rowKey :">row index</param>
    /// <param name="columnKey :">column key</param>
    /// <returns>실패시 FIND_STR_FAIL을 반환합니다.</returns>
    std::string_view FindData(const std::string& rowKey, const std::string& columnKey);

private:
    friend class ExcelDataSystem;
    ExcelDataBase(const DataBaseType& dataBase);
    const DataBaseType& _dataBase;
};

class ExcelDataSystem : public Component
{
    USING_PROPERTY(ExcelDataSystem)

public:
    ExcelDataSystem();
    ~ExcelDataSystem() override;

public:
    using ExcelSystemMapType = std::unordered_map<std::string, ExcelDataBase::DataBaseType>;

    /// <summary>
    /// 시트 이름을 키값으로 엑셀 데이터 베이스를 반환합니다. 없으면 nullptr을 반환합니다.
    /// </summary>
    /// <param name="sheetName :">원하는 시트 이름</param>
    /// <returns>동적 할당된 ExcelDataBase</returns>
    std::unique_ptr<ExcelDataBase> FindExcelDataBase(const std::string& sheetName);

private:
    SingletonObject<ExcelDataSystem> _singletonObject;

public:
    REFLECT_PROPERTY()
protected:
    REFLECT_FIELDS_BEGIN(Component)
    ExcelSystemMapType DataBase;
    REFLECT_FIELDS_END(ExcelDataSystem)

    void Reset() override;
    void Awake() override;
    void ImGuiDrawPropertysEvent() override;

private:
#ifdef _UMEDITOR
    ImGuiColumnSheetParser _excelParser;
    bool                   _drawDataBaseView = false;
#endif
    void ImGuiDrawExcelParserEdit();
    void ImGuiDrawDataSheetView();

};
