#include "pchScripts.h"
#include "QTEEditor.h"
#include <QTE/System/QTESystem.h>

QTEEditor::QTEEditor() 
{
    int flags = Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU |
                Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_CONTEXT_MENU |
                Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CONTEXT |
                Timeline::SequencerEditor::FLAGS_HIDE_CURRENT_LINE |
                Timeline::SequencerEditor::FLAGS_HIDE_MIN_MAX_LINE |
                Timeline::SequencerEditor::FLAGS_HIDE_CONTEXT_LABEL |
                Timeline::SequencerEditor::FLAGS_DRAW_CONTEXT_LINE_VERTICAL ;
    _sequencerEditor.AddFlags(flags);
    _sequencerEditor.SetEventTrack(_qteTrack.GetEventTrack());
    auto& callback           = _sequencerEditor.GetCallback();
    callback.LowerFramePopup = [this](Timeline::EventTrack* track) {
        if (ImGui::MenuItem("Add Note"))
        {
            float min, max, frame;
            min     = track->GetMinFrame();
            max     = track->GetMaxFrame();
            frame   = _sequencerEditor.GetFrameFromIndicate();
            track->AddEvent<QTE::Note>("Note", ImClamp(frame, min, max));
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
            ImVec2 availSize  = ImGui::GetContentRegionAvail();
            ImVec2 canvasSize = ImVec2(availSize.x * 0.2f, availSize.y);
            // QTE 시스템 정보
            ImGui::BeginChild("system_detail", canvasSize, true, ImGuiChildFlags_Border);
            {
                ImVec2     availSize  = ImGui::GetContentRegionAvail();
                float      labelWidth = availSize.x * 0.3f;
                QTESystem* system = QTESystem::GetInstance();
                if (system)
                {
                    // 스피드
                    float speed = system->GetQTESpeedScale();
                    ImGuiHelper::TextWithVerticalSeparator("QTE Speed Scale", labelWidth);
                    ImGuiHelper::HoveredToolTip("QTE 글로벌 속도 배율입니다.");
                    if (ImGui::DragFloat("##qte_speed", &speed, 0.01f, 0.1f, 10.0f))
                    {
                        system->SetQTESpeedScale(speed);
                    }

                    // 대기시간
                    float delay = system->GetDelayFromQTEStart();
                    ImGuiHelper::TextWithVerticalSeparator("QTE Delay Second", labelWidth);
                    ImGuiHelper::HoveredToolTip("QTE 시작 후 대기 시간(초)입니다.");
                    if (ImGui::DragFloat("##qte_delay", &delay, 0.01f, 0.0f, 10.0f))
                    {
                        system->SetDelayFromQTEStart(delay);
                    }
                }
                else
                {
                    ImGui::Text("No QTE System in Scene");
                }
            }
            ImGui::EndChild();

            // QTE 트랙 정보
            ImGui::SameLine();
            canvasSize = ImGui::GetContentRegionAvail();
            ImGui::BeginChild("track_detail", canvasSize, true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiChildFlags_Border);
            {
                {
                    std::string pathStr     = _qteTrack.GetFilePath().generic_string();
                    ImVec2      buttonSize  = ImVec2(ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().y);
                    float       fontSize    = ImGui::GetFontSize();
                    float       frameHeight = ImGui::GetFrameHeight();
                    float       height      = ImGui::GetTextLineHeightWithSpacing();
                    ImGui::BeginChild("##path_frame", ImVec2(0, fontSize + height), ImGuiChildFlags_Border);
                    ImGuiHelper::TextWithVerticalSeparator("QTE File Path");
                    // ImGui::BeginDisabled();
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - (buttonSize.y * 3.0f) - 100.0f);
                    ImGui::InputText("##path_input", &pathStr,
                                     ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
                    // ImGui::EndDisabled();
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
                    auto track = _qteTrack.GetEventTrack().lock();
                    if (track)
                    {
                        ImVec2 availSize  = ImGui::GetContentRegionAvail();
                        float  labelWidth = ImClamp(availSize.x * 0.2f, 30.0f, 150.0f);

                        // MinFrame
                        float minFrame = track->GetMinFrame();
                        ImGuiHelper::TextWithVerticalSeparator("Min Frame", labelWidth);
                        if (ImGui::InputFloat("##min_frame", &minFrame))
                        {
                            track->SetMinFrame(minFrame);
                        }

                        // MaxFrame
                        float maxFrame = track->GetMaxFrame();
                        ImGuiHelper::TextWithVerticalSeparator("Max Frame", labelWidth);
                        if (ImGui::InputFloat("##max_frame", &maxFrame))
                        {
                            track->SetMaxFrame(maxFrame);
                        }

                        // Sequencer
                        {
                            ImVec2 availSize  = ImGui::GetContentRegionAvail();
                            ImVec2 canvasSize = ImVec2(availSize.x * 0.8f, availSize.y);
                            ImGui::BeginChild("sequencer_frame", canvasSize, true,
                                              ImGuiWindowFlags_NoScrollWithMouse | ImGuiChildFlags_Border);
                            _sequencerEditor.Show();
                            ImGui::EndChild();
                        }

                        // detail
                        {
                            ImGui::SameLine();
                            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
                            ImGui::BeginChild("DetailFrame", canvasSize, true, ImGuiChildFlags_Border);
                            if (ImGui::BeginTabBar("##AnimationTrackTabs"))
                            {
                                if (ImGui::BeginTabItem("dummy tab1"))
                                {
                                    UINT selected = _sequencerEditor.GetSelectedContextID();
                                    auto context  = track->GetContextFromID(selected);
                                    if (nullptr != context)
                                    {
                                        ImGuiHelper::AlignedText("Note Data", ImGuiHelper::LEFT, 0.8f);

                                        ImGui::PushID(track.get());
                                        std::string_view label      = context->Label;
                                        float            time       = context->Time;
                                        UINT             id         = context->ID;
                                        ImVec2           availSize  = ImGui::GetContentRegionAvail();
                                        float            labelWidth = availSize.x * 0.2f;
                                        ImGuiHelper::TextWithVerticalSeparator("Label", labelWidth);
                                        std::string labelStr(label);
                                        if (ImGui::InputText("##track_label", &labelStr))
                                        {
                                            context->SetLabel(labelStr);
                                        }
                                        ImGuiHelper::TextWithVerticalSeparator("Time", labelWidth);
                                        if (ImGui::InputFloat("##track_time", &time, 0, 0))
                                        {
                                            float minFrame = track->GetMinFrame();
                                            float maxFrame = track->GetMaxFrame();
                                            time           = ImClamp(time, minFrame, maxFrame);
                                            track->ChangeContextTime(selected, time);
                                        }
                                        ImGui::Separator();
                                        ImGuiHelper::AlignedText("Extra Data", ImGuiHelper::LEFT, 0.8f);
                                        context->ImGuiDrawPropertys();
                                        ImGui::PopID();
                                    }
                                    ImGui::EndTabItem();
                                }
                                if (ImGui::BeginTabItem("dummy tab2"))
                                {
                                    ImGui::EndTabItem();
                                }
                                ImGui::EndTabBar();
                            }
                            ImGui::EndChild();
                        }
                    }
                    ImGui::EndChild();
                }
            }
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
