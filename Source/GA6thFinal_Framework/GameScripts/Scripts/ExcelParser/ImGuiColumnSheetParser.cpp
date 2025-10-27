#include "pchScripts.h"
#include "ImGuiColumnSheetParser.h"

using namespace u8_literals;

namespace
{
    constexpr unsigned int ROW_MAX = std::numeric_limits<unsigned int>::max();
}

ImGuiColumnSheetParser::ImGuiColumnSheetParser(std::string_view imguiID, std::u8string_view keyValue)
{
    _lastOpenFolder = File::GetDesktopPath();
    _id = "Excel Parser##";
    _id += imguiID;
    _keyRowIndex = ROW_MAX;
    _keyValue    = (const char*)keyValue.data();
}

ImGuiColumnSheetParser::ImGuiColumnSheetParser(std::string_view imguiID, unsigned int keyRowIndex) 
{
    _lastOpenFolder = File::GetDesktopPath();
    _id = "Excel Parser##";
    _id += imguiID;
    _keyRowIndex = keyRowIndex;
}

bool ImGuiColumnSheetParser::Draw(const std::function<void(const ColumnDatas&)>& callBackFunc)
{
    bool result = false;
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
                if (ImGui::BeginCombo("##Sheet Box", _selectSheetName.c_str()))
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

                ImGui::InputText("Key value", &_keyValue);
                ImGuiHelper::HoveredToolTip(u8"키로 사용되는 값을 하나만 입력하세요.");

                if (ImGui::Button("Ok") && false == _selectSheetName.empty())
                {
                    auto& doc       = *_excelDoc;
                    auto  workBook  = doc.workbook();
                    auto  workSheet = workBook.worksheet(_selectSheetName.c_str());

                    unsigned int keyRow = _keyRowIndex;
                    if (keyRow == ROW_MAX && false == _keyValue.empty())
                    {
                        keyRow = OpenXLSXHelper::FindRowColumnToData(workSheet, _keyValue).first;
                    }
                    if (keyRow != OpenXLSXHelper::FAIL_ROW)
                    {
                        // 파싱
                        _sheetDatas = OpenXLSXHelper::ParseSheetWithColumnKeys(workSheet, keyRow, ParserThreshold);
                        if (callBackFunc)
                        {
                            Apply(callBackFunc);
                        }
                        result = true;
                    }
                }
                ImGui::SameLine();
                ImGui::DragInt("Pareser Threshold", reinterpret_cast<int*>(&ParserThreshold), 1.0f, 1, 10);
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

    return result;
}

bool ImGuiColumnSheetParser::Apply(const std::function<void(const ColumnDatas& datas)>& callBackFunc)
{
    if (false == _sheetDatas.empty())
    {
        ColumnDatas columnDatas;
        for (size_t row = 0; row < _sheetDatas.front().second.size(); ++row)
        {
            columnDatas.clear();
            bool emptyColumns = true;
            for (auto& [key, datas] : _sheetDatas)
            {
                auto& data = datas[row];
                emptyColumns &= data.empty();
                columnDatas.emplace_back(std::cref(key), std::cref(data));
            }

            if (false == emptyColumns && false == columnDatas.empty())
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
            std::vector<File::Path> out;
            if (File::ShowOpenFileDialog(NULL, L"로드할 파일을 선택하세요.", _lastOpenFolder.c_str(),
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
                        _lastOpenFolder        = out.front().parent_path().generic_wstring();
                    }
                }
            }
        }
        ImGui::EndMenuBar();
    }
}
