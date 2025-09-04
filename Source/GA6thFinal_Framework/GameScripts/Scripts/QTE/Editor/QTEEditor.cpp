#include "pchScripts.h"
#include "QTEEditor.h"

QTEEditor::QTEEditor() 
{
    _sequencerEditor.SetEventTrack(_qteTrack.GetEventTrack());
    _sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_ALL_INPUT);
    auto& callback           = _sequencerEditor.GetCallback();
    callback.LowerFramePopup = [this](Timeline::EventTrack* editor) {
        if (ImGui::MenuItem("Add Note"))
        {
            editor->AddEvent<QTE::Note>("Note", editor->GetCurrentFrame());
        }
    };
}

QTEEditor::~QTEEditor() 
{
}

void QTEEditor::Show()
{
    if (_editorOpened)
    {
        ImGui::Begin("QTE Editor", &_editorOpened, ImGuiWindowFlags_NoMove);
        ImRect tabRect  = ImGuiHelper::GetWindowTabBarRect();
        ImVec2 mousePos = ImGui::GetMousePos();

        int state = _dragHandler.BeginDragState("WindowTab", tabRect, mousePos, ImGuiMouseButton_Left);
        switch (state)
        {
        case EditorDragState::DRAG_STATE_DRAGGING: {
            ImVec2 newPos = tabRect.Min + ImGui::GetIO().MouseDelta;
            ImGui::SetWindowPos(newPos);
            break;
        }
        default:
            break;
        }

        {
            std::string pathStr     = _qteTrack.GetFilePath().generic_string();
            ImVec2      buttonSize  = ImVec2(ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().y);
            float       fontSize    = ImGui::GetFontSize();
            float       frameHeight = ImGui::GetFrameHeight();
            float       height      = ImGui::GetTextLineHeightWithSpacing();
            ImGui::BeginChild("##path_frame", ImVec2(0, fontSize + height), ImGuiChildFlags_Border);
            ImGuiHelper::TextWithVerticalSeparator("QTE File Path");
            //ImGui::BeginDisabled();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (buttonSize.y * 3.0f) - 50.0f);
            ImGui::InputText("##path_input", &pathStr, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
            //ImGui::EndDisabled();
            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text(pathStr.c_str());
                ImGui::EndTooltip();
            }
            ImGui::SameLine();
            if (ImGui::Button(EditorIcon::ICON_FILE, buttonSize))
            {
                NewFileWithDialog();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("New QTE File");

            ImGui::SameLine();
            if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN, buttonSize))
            {
                LoadFileWithDialog();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Open QTE File");

            ImGui::SameLine();
            if (ImGui::Button(EditorIcon::ICON_FILE_SAVE, buttonSize))
            {
                const File::Path& filePath = _qteTrack.GetFilePath();
                if (true == filePath.IsNull())
                {
                    SaveFileWithDialog();
                }
                else
                {
                    _qteTrack.SaveFile(filePath, true);
                }
            }
            ImGui::EndChild();
        }
        
        {
            ImGui::BeginChild("##edit_frame", ImVec2(0, 0), ImGuiChildFlags_Border);
            _sequencerEditor.Show();
            ImGui::EndChild();
        }

        ImGui::End();
    }
}

void QTEEditor::Open() 
{
    _editorOpened = true;
}

bool QTEEditor::NewFileWithDialog()
{
    File::Path                                  out;
    HWND                                        owner       = UmApplication.GetHwnd();
    LPCWSTR                                     title       = L"새 QTE 파일 만들기";
    LPCWSTR                                     defaultPath = UmFileSystem.GetRootPath().c_str();
    LPCWSTR                                     defaultName = QTE::Track::DEFAULT_NAME.c_str();
    std::vector<std::pair<LPCWSTR, LPCWSTR>>    filters     = {{L"QTE 트랙 파일\0", L"*.UmQTETrack\0"}};
    if (File::ShowSaveFileDialog(owner, title, defaultPath, defaultName, filters, out))
    {
        return _qteTrack.NewFile(out);
    }
    return false;
}

bool QTEEditor::LoadFileWithDialog()
{
    std::vector<File::Path>                     out;
    HWND                                        owner       = UmApplication.GetHwnd();
    LPCWSTR                                     title       = L"QTE 파일 열기";
    LPCWSTR                                     defaultPath = UmFileSystem.GetRootPath().c_str();
    std::vector<std::pair<LPCWSTR, LPCWSTR>>    filters     = {{L"QTE 트랙 파일\0", L"*.UmQTETrack\0"}, {L"All File\0", L"*.*\0"}};
    if (File::ShowOpenFileDialog(owner, title, defaultPath, filters, false, out))
    {
        return _qteTrack.LoadFile(out.front());
    }
    return false;
}

bool QTEEditor::SaveFileWithDialog()
{
    File::Path                                  out;
    HWND                                        owner        = UmApplication.GetHwnd();
    LPCWSTR                                     title        = L"QTE 파일 저장";
    LPCWSTR                                     defaultPath  = UmFileSystem.GetRootPath().c_str();
    LPCWSTR                                     defaultName  = QTE::Track::DEFAULT_NAME.c_str();
    if (File::ShowSaveFileDialog(owner, title, defaultPath, defaultName, {}, out))
    {
        return _qteTrack.SaveFile(out);
    }
    return false;
}
