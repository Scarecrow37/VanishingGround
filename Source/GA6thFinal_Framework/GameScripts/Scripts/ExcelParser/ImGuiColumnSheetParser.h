#pragma once
#include "UmFramework.h"

class ImGuiColumnSheetParser
{
public:
    using ColumnDatas = std::vector<std::pair<std::reference_wrapper<const std::string>, std::reference_wrapper<const std::string>>>;
    using ParsedDatas = std::vector<std::pair<std::string, std::vector<std::string>>>;
    ImGuiColumnSheetParser(std::string_view id);

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
private:
    std::string                           _id;
    std::unique_ptr<OpenXLSX::XLDocument> _excelDoc;
    std::vector<std::string>              _sheetNames;
    std::string                           _selectSheetName;
    ParsedDatas                           _sheetDatas;

    void DrawMenuBar();
};