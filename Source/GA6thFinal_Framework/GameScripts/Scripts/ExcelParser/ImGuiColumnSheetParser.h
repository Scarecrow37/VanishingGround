#pragma once
#include "UmFramework.h"

class ImGuiColumnSheetParser
{
public:
    ImGuiColumnSheetParser(std::string_view id);

    /// <summary>
    /// 파서를 Draw 합니다. 파싱 이후 파잉된 데이터들을 순회하면서 key, data를 사용할 콜백 함수를 인자로 넘겨주어야 합니다.
    /// </summary>
    /// <param name="callback"></param>
    void Draw(const std::function<void (const std::string&, const std::string&)>& callBackFunc);

    /// <summary>
    /// 파서의 ImGui::Begin에 연결된 bool 값입니다. 
    /// </summary>
    bool ShowParser = false;
private:
    std::string _id;
    std::unique_ptr<OpenXLSX::XLDocument>                         _excelDoc;
    std::vector<std::string>                                      _sheetNames;
    std::string                                                   _selectSheetName;
    std::vector<std::pair<std::string, std::vector<std::string>>> _sheetDatas;

    void DrawMenuBar();
};