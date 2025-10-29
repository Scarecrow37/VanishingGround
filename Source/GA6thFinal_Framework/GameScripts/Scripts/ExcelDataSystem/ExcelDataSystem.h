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
    size_t FindColumnIndex(std::u8string_view columnKeyName);

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
    size_t FindRowIndex(std::u8string_view rowKey, size_t columnIndex);

    /// <summary>
    /// 해당 column key에 row key의 인덱스를 반환합니다.
    /// </summary>
    /// <param name="rowKey :">찾을 문자열 데이터</param>
    /// <param name="columnIndex :">탐색할 column 키 index</param>
    /// <returns>실패시 ExcelDataBase::FIND_INDEX_FAIL</returns>
    size_t FindRowIndex(std::u8string_view rowKey, std::u8string_view columnKey);

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
    /// <param name="rowIndex :">row index</param>
    /// <param name="columnKey :">column key</param>
    /// <returns>실패시 FIND_STR_FAIL을 반환합니다.</returns>
    std::string_view FindData(size_t rowIndex, std::u8string_view columnKey);

    /// <summary>
    /// 데이터 시트에서 Row의 Column 데이터들을 가져옵니다.
    /// </summary>
    /// <param name="rowIndex :">가져올 데이터의 Row 인덱스</param>
    /// <returns>실패시 nullptr</returns>
    const std::vector<std::string>* GetColumnDatas(size_t rowIndex);

    /// <summary>
    /// 데이터 시트에서 해당 columnIndex에 rowKey 데이터가 있는 동일한 Row의 모든 데이터를 반환합니다.
    /// </summary>
    /// <param name="rowKey"></param>
    /// <param name="columnIndex"></param>
    /// <returns></returns>
    const std::vector<std::string>* GetColumnDatas(std::u8string_view rowKey, size_t columnIndex);

    /// <summary>
    /// 데이터 시트에서 해당 columnKey에 rowKey 데이터가 있는 동일한 Row의 모든 데이터를 반환합니다.
    /// </summary>
    /// <param name="rowKey :">가져올 데이터 Row 인덱스를 찾기 위한 문자열y</param>
    /// <param name="columnKey :">찾을 Row 데이터가 존재하는 Column key</param>
    /// <returns></returns>
    const std::vector<std::string>* GetColumnDatas(std::u8string_view rowKey, std::u8string_view columnKey);

    /// <summary>
    /// Row Count
    /// </summary>
    /// <returns>Row Count</returns>
    size_t RowCount() const;
    
    /// <summary>
    /// Column Count
    /// </summary>
    /// <returns>Column Count</returns>
    size_t ColumnCount();

private:
    friend class ExcelDataSystem;
    ExcelDataBase(const std::string& dataBaseKey);
    const std::u8string _key;

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
    /// 시트 이름을 키값으로 엑셀 데이터베이스를 반환합니다. 없으면 nullptr을 반환합니다.
    /// </summary>
    /// <param name="sheetName :">원하는 시트 이름</param>
    /// <returns>동적 할당된 ExcelDataBase</returns>
    std::unique_ptr<ExcelDataBase> FindExcelDataBase(std::u8string_view sheetName);

    /// <summary>
    /// 원본 데이터 베이스의 포인터를 반환합니다. 없으면 nullptr을 반환합니다. (데이터를 찾을때는 FindExcelDataBase를 이용해야합니다.)
    /// </summary>
    /// <param name="sheetName :">원하는 시트 이름</param>
    /// <returns>해당 시트 데이터 베이스의 포언터</returns>
    const ExcelDataBase::DataBaseType* GetRowDataBase(std::u8string_view sheetName);

private:
    SingletonObject<ExcelDataSystem> _singletonObject;
    SingletonComponent<ExcelDataSystem> _singletonComponent;

public:
    REFLECT_PROPERTY()
protected:
    REFLECT_FIELDS_BEGIN(Component)
    ExcelSystemMapType DataBase;
    REFLECT_FIELDS_END(ExcelDataSystem)

    void Reset() override;
    void Added() override;
    void ImGuiDrawPropertysEvent() override;

private:
#ifdef _UMEDITOR
    ImGuiColumnSheetParser _excelParser;
    bool                   _drawDataBaseView = false;
#endif
    void ImGuiDrawExcelParserEdit();
    void ImGuiDrawDataSheetView();
    void ImGuiDrawFindTest(const std::string& viewerSheetName);
};
