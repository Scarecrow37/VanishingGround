#include "pchScripts.h"
#include "QTEEditor.h"
#include <QTE/System/QTESystem.h>

QTEEditor::QTEEditor() 
{
    int flags = Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE |
                Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_MIN_MAX_LINE |
                Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CONTEXT |
                Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU |
                Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_CONTEXT_MENU |
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
            ShowSystemDetail();
            ImGui::SameLine();
            ShowTrackDetail();
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

void QTEEditor::ShowSystemDetail()
{
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(availSize.x * 0.2f, availSize.y);
    // QTE 시스템 정보
    ImGui::BeginChild("system_detail", canvasSize, ImGuiChildFlags_Border);
    {
        ImVec2     availSize  = ImGui::GetContentRegionAvail();
        float      labelWidth = availSize.x * 0.3f;
        QTESystem* system     = QTESystem::GetInstance();
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
}

void QTEEditor::ShowTrackDetail() 
{
    // QTE 트랙 정보
    ImGui::SameLine();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("track_detail", canvasSize, ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollWithMouse);
    {
        // File Path
        ShowFilePathFrame();

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

            // Sequencer Frame
            ShowSequencerFrame();

            // Preview Frame
            ShowPreviewFrame();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

void QTEEditor::ShowFilePathFrame() 
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
    ImGui::InputText("##path_input", &pathStr, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
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

void QTEEditor::ShowSequencerFrame() 
{
    auto track = _qteTrack.GetEventTrack().lock();
    if (track)
    {
        float heightScale = 0.6f;
        // Sequencer
        {
            ImVec2 availSize  = ImGui::GetContentRegionAvail();
            ImVec2 canvasSize = ImVec2(availSize.x * 0.6f, availSize.y * heightScale);
            ImGui::BeginChild("sequencer_frame", canvasSize, ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollWithMouse);
            _sequencerEditor.Show();
            ImGui::EndChild();
        }

        // detail
        {
            ImGui::SameLine();
            ImVec2 availSize  = ImGui::GetContentRegionAvail();
            ImVec2 canvasSize = ImVec2(availSize.x, availSize.y * heightScale);
            ImGui::BeginChild("sequencer_detail_frame", canvasSize, true);
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
}

void QTEEditor::ShowPreviewFrame() 
{
    auto system = QTESystem::GetInstance();
    auto track  = _qteTrack.GetEventTrack().lock();
    if (system && track)
    {
        ImGui::BeginChild("preview_frame", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_None);

        float  globalSpeedScale = system->GetQTESpeedScale();
        float  trackSpeedScale  = _qteTrack.GetQTESpeedScale();
        float  maxFrame         = track->GetMaxFrame();
        float  minFrame         = track->GetMinFrame();
        ImVec2 availSize        = ImGui::GetContentRegionAvail();
        ImVec2 buttonSize       = ImVec2(ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().y); // 정사각형 버튼
        float  spacing          = ImGui::GetStyle().ItemSpacing.x;

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

        if (ImGui::Button(EditorIcon::ICON_PLAY, buttonSize))
        {
            _isPreviewPlaying = true;
            track->SetCurrentFrame(_previewTimer, true);
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Play");

        ImGui::SameLine();
        if (ImGui::Button(EditorIcon::ICON_PAUSE, buttonSize))
        {
            _isPreviewPlaying = false;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Pause");

        ImGui::SameLine();
        if (ImGui::Button(EditorIcon::ICON_STOP, buttonSize))
        {
            _previewTimer     = 0.0f;
            _isPreviewPlaying = false;
            track->SetCurrentFrame(_previewTimer, true);
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Stop");

        ImGui::PopStyleVar(); // 라운딩 원복

        // 타이머
        ImGui::Text("Time: %.2f s", _previewTimer);

        DrawPreview();
        if (_isPreviewPlaying)
        {
            _sequencerEditor.RemoveFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
            _previewTimer += ImGui::GetIO().DeltaTime * trackSpeedScale * globalSpeedScale;
            track->SetCurrentFrame(_previewTimer, true);
            if (_previewTimer >= maxFrame)
            {
                _isPreviewPlaying = false;
                _previewTimer     = minFrame;
                track->SetCurrentFrame(_previewTimer, true);
            }
        }
        else
        {
            _sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
            _previewTimer = track->GetCurrentFrame();
        }
        
        ImGui::EndChild();
    }
}

void QTEEditor::DrawPreview() 
{

    ImGui::BeginChild("##preview_canvas", ImVec2(0, 0), ImGuiChildFlags_Border);
    auto* window = ImGui::GetCurrentWindow();
    if (window && window->DrawList)
    {
        static float circleRadius     = 30.0f; // 노트 원의 반지름
        static float perfectPosFactor = 0.8f; // 전체 길이의 몇 퍼센트 지점이 퍼펙트 위치인지
        auto*  drawList  = window->DrawList;
        ImVec2 offset    = ImGui::GetCursorScreenPos();
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        auto   system    = QTESystem::GetInstance();
        auto   track     = _qteTrack.GetEventTrack().lock();

        float  perfectPosX = availSize.x * perfectPosFactor;

        drawList->AddRectFilled(offset, offset + availSize, IM_COL32(30, 30, 30, 255));

        drawList->AddRectFilled(offset + ImVec2(perfectPosX - circleRadius, 0.0f), offset + ImVec2(perfectPosX + circleRadius, availSize.y), IM_COL32(100, 255, 100, 100));
        drawList->AddCircleFilled(offset + ImVec2(availSize.x * perfectPosFactor, availSize.y * 0.5f), circleRadius, IM_COL32(100, 255, 100, 255));

        if (system && track)
        {
            
            float delay       = system->GetDelayFromQTEStart();
            for (const auto& note : track->GetEventContextQueue())
            {
                float noteTime = note->Time + delay;
                float posX     = (1.0f + _previewTimer - noteTime) * perfectPosX;
                if (posX > availSize.x)
                {
                    continue;
                }
                drawList->AddRectFilled(offset + ImVec2(posX - circleRadius, 0.0f), offset + ImVec2(posX + circleRadius, availSize.y), IM_COL32(100, 100, 255, 100));
                drawList->AddCircleFilled(offset + ImVec2(posX, availSize.y * 0.5f), circleRadius, IM_COL32(100, 100, 255, 255));
            }
        }
    }
    
    ImGui::EndChild();

    //ImVec2 availSize = ImGui::GetContentRegionAvail();
    //ImVec2 pos       = ImGui::GetCursorScreenPos();
    //ImVec2 size      = ImVec2(availSize.x, availSize.y - ImGui::GetFrameHeightWithSpacing());
    //// 배경
    //ImU32 bgColor = IM_COL32(30, 30, 30, 255);
    //ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor);
    //// 타이머 표시
    //std::string timerStr = std::format("Time: {:.2f}s", _previewTimer);
    //ImVec2     textSize  = ImGui::CalcTextSize(timerStr.c_str());
    //ImVec2     textPos   = ImVec2(pos.x + (size.x - textSize.x) * 0.5f, pos.y + (size.y - textSize.y) * 0.5f);
    //ImGui::GetWindowDrawList()->AddText(textPos, IM_COL32(255, 255, 255, 255), timerStr.c_str());
}
