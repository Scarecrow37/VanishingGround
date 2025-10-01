#pragma once
#include "UmFramework.h"

class ImGuiColumnSheetParser
{
public:
    using ColumnDatas = std::vector<std::pair<std::reference_wrapper<const std::string>, std::reference_wrapper<const std::string>>>;
    using ParsedDatas = std::vector<std::pair<std::string, std::vector<std::string>>>;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="imguiID :">ImGui ID로 사용할 번호</param>
    /// <param name="keyValue :">엑셀 데이터에서 하나의 Key 값. 해당 키와 동일한 Row 값들이 Key가 됩니다.</param>
    ImGuiColumnSheetParser(std::string_view imguiID, std::u8string_view keyValue);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="imguiID :">ImGui ID로 사용할 번호</param>
    /// <param name="keyRowIndex">엑셀 데이터에서 Key 부분에 해당하는 Row Index</param>
    ImGuiColumnSheetParser(std::string_view imguiID, unsigned int keyRowIndex);

    /// <summary>
    /// 파서를 Draw 합니다. 파싱 이후 파싱된 데이터들을 순회하면서 key, data를 사용할 콜백 함수를 인자로 넘겨주어야 합니다.
    /// 파싱이 완료되었을때 true를 1회 반환합니다. 이후 GetSheetDats를 사용해 값을 사용할 수 있습니다.
    /// </summary>
    /// <param name="callback"></param>
    bool Draw(const std::function<void(const ColumnDatas& datas)>& callBackFunc = nullptr);

    /// <summary>
    /// 파싱된 데이터를 순회합니다. Draw에 의한 파싱이 이루어진적이 있어야만 사용 가능합니다.
    /// </summary>
    /// <returns>성공 여부.</returns>
    bool Apply(const std::function<void(const ColumnDatas& datas)>& callBackFunc);

    /// <summary>
    /// 파싱된 데이터들을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const ParsedDatas& GetParsedDatas() const { return _sheetDatas; }

    /// <summary>
    /// 선택된 시트 이름을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::string& GetSelectSheetName() const { return _selectSheetName; }

    size_t GetColumnCount() const 
    { 
        if (false == _sheetDatas.empty())
        {
            return _sheetDatas.size();
        }
        return 0;
    }

    size_t GetRawCount() const
    {
        if (false == _sheetDatas.empty())
        {      
            return _sheetDatas.front().second.size();
        }
        return 0;
    }

    /// <summary>
    /// 파싱된 데이터들을 정리합니다. (*ShowParser가 false가 되면 자동으로 정리됩니다.)
    /// </summary>
    void ClearParsedDatas() 
    { 
        _selectSheetName.clear();
        _sheetNames.clear();
        _sheetDatas.clear();
    }

    /// <summary>
    /// 파서의 ImGui::Begin에 연결된 bool 값입니다. false일때 로드된 데이터가 전부 Clear 됩니다.
    /// </summary>
    bool ShowParser = false;

    /// <summary>
    /// 파서의 빈 데이터 허용치 입니다.
    /// </summary>
    unsigned int ParserThreshold = 3;
private:
    std::string                           _id;
    std::wstring                          _lastOpenFolder;
    std::unique_ptr<OpenXLSX::XLDocument> _excelDoc;
    std::vector<std::string>              _sheetNames;
    std::string                           _selectSheetName;
    ParsedDatas                           _sheetDatas;
    unsigned int                          _keyRowIndex;
    std::string                           _keyValue;

    void DrawMenuBar();
};