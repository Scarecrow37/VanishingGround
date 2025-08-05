#include "pchScripts.h"
#include "ImGuiColumnSheetParser.h"

using namespace u8_literals;

ImGuiColumnSheetParser::ImGuiColumnSheetParser(std::string_view id) 
{
    _id = "Excel Parser##";
    _id += id;
}

void ImGuiColumnSheetParser::Draw(const std::function<void(const ColumnDatas&)>& callBackFunc)
{
    if (ShowParser)
    {
        ImGui::Begin(_id.c_str(), &ShowParser,
                     ImGuiWindowFlags_MenuBar);
        {
            ImGui::PushID(this);
            DrawMenuBar();

            if (true == _sheetNames.empty())
            {
                ImGui::Text((const char*)u8"엑셀 파일을 로드해주세요.");
            }
            else
            {
                ImGui::Text(u8"파싱할 시트를 선택하세요."_c_str);
                if (ImGui::BeginCombo("##{A4CAA356-B858-4BFF-85E8-52E3B270A7D2}", _selectSheetName.c_str()))
                {
                    for (auto& name : _sheetNames)
                    {
                        if (ImGui::Selectable(name.c_str(), _selectSheetName == name))
                        {
                            _selectSheetName = name;
                        }
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Ok") && false == _selectSheetName.empty())
                {
                    auto& doc       = *_excelDoc;
                    auto  workBook  = doc.workbook();
                    auto  workSheet = workBook.worksheet(_selectSheetName.c_str());

                    auto [keyRow, keyColum] = OpenXLSXHelper::FindRowColumnToData(workSheet, u8"이름"_c_str);
                    if (OpenXLSXHelper::IsFindSuccess(keyRow, keyColum))
                    {
                        // 파싱
                        _sheetDatas = OpenXLSXHelper::ParseSheetWithColumnKeys(workSheet, keyRow);
                        if (callBackFunc)
                        {
                            Apply(callBackFunc);
                        }
                    }
                }
            }
            ImGui::PopID();
        }
        ImGui::End();
    }

    if (_excelDoc && ShowParser == false)
    {
        if (_excelDoc->isOpen())
        {
            _excelDoc->close();
        }
        _excelDoc.reset();
        _sheetNames.clear();
        _selectSheetName.clear();
        _sheetDatas.clear();
    }
}

bool ImGuiColumnSheetParser::Apply(const std::function<void(const ColumnDatas& datas)>& callBackFunc)
{
    if (false == _sheetDatas.empty())
    {
        ColumnDatas columnDatas;
        for (size_t row = 0; row < _sheetDatas.front().second.size(); ++row)
        {
            columnDatas.clear();
            for (auto& [key, datas] : _sheetDatas)
            {
                columnDatas.emplace_back(std::cref(key), std::cref(datas[row]));
            }

            if (false == columnDatas.empty())
            {
                callBackFunc(columnDatas);
            }
        }
        return true;
    }
    return false;
}

void ImGuiColumnSheetParser::DrawMenuBar() 
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::MenuItem("Load Excel Table"))
        {
            std::wstring_view       desktopPath = File::GetDesktopPath();
            std::vector<File::Path> out;
            if (File::ShowOpenFileDialog(NULL, L"로드할 파일을 선택하세요.", desktopPath.data(),
                                         {{L"테이블 파일\0", L"*.xlsm\0"}}, false, out))
            {
                if (false == out.empty())
                {
                    if (nullptr == _excelDoc)
                    {
                        _excelDoc.reset(new OpenXLSX::XLDocument);
                    }
                    _sheetNames.clear();
                    _selectSheetName.clear();
                    _sheetDatas.clear();
                    _excelDoc->open(out.front().generic_string());
                    auto& doc = *_excelDoc;
                    if (doc.isOpen())
                    {
                        auto workBook          = doc.workbook();
                        _sheetNames = workBook.sheetNames();
                    }
                }
            }
        }
        ImGui::EndMenuBar();
    }
}
