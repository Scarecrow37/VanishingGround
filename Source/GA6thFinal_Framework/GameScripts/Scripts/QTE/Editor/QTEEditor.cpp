#include "pchScripts.h"
#include "QTEEditor.h"
#include <QTE/System/QTESystem.h>
#include <QTE/Editor/QTEPreviewer.h>
#include <WeaponSystem/WeaponSystem.h>
#include <WeaponSystem/WeaponTable/WeaponTableComponent.h>

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
    ImVec2 canvasSize = ImVec2(availSize.x * 0.3f, availSize.y);
    // QTE 시스템 정보
    ImGui::BeginChild("system_detail", canvasSize, ImGuiChildFlags_Border);
    {
        ImVec2     availSize  = ImGui::GetContentRegionAvail();
        float      labelWidth = availSize.x * 0.5f;
        QTESystem* system     = QTESystem::GetInstance();
        if (system)
        {
            // 스피드
            float speed = system->GetQTESpeedScale();
            ImGuiHelper::TextWithVerticalSeparator("QTE Speed Scale", labelWidth);
            if (ImGui::DragFloat("##qte_speed", &speed, 0.01f, 0.1f, 10.0f))
            {
                system->SetQTESpeedScale(speed);
            }
            ImGuiHelper::HoveredToolTip((const char*)u8"QTE 글로벌 속도 배율입니다.");

            // 대기시간
            float delay = system->GetDelayFromQTEStart();
            ImGuiHelper::TextWithVerticalSeparator("QTE Delay Second", labelWidth);
            if (ImGui::DragFloat("##qte_delay", &delay, 0.01f, 0.0f, 10.0f))
            {
                system->SetDelayFromQTEStart(delay);
            }
            ImGuiHelper::HoveredToolTip((const char*)u8"QTE 시작 후 대기 시간(초)입니다.");

            // 퍼펙트 판정 범위
            {
                auto [min, max] = system->GetPerfectJudgeRange();
                // ms 단위로 변환
                min *= 1000.0f;
                max *= 1000.0f;
                ImGuiHelper::TextWithVerticalSeparator("QTE Perfect Judge Range", labelWidth);
                if (ImGui::DragFloat2("##qte_perfect_range", &min, 0.01f))
                {
                    // s 단위로 변환
                    system->SetPerfectJudgeRange(min * 0.001f, max * 0.001f);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 퍼펙트 판정 범위입니다. 단위: ms");
            }

            // 일격 판정 범위
            {
                auto [min, max] = system->GetNormalJudgeRange();
                // ms 단위로 변환
                min *= 1000.0f;
                max *= 1000.0f;
                ImGuiHelper::TextWithVerticalSeparator("QTE Normal Judge Range", labelWidth);
                if (ImGui::DragFloat2("##qte_normal_range", &min, 0.01f))
                {
                    // s 단위로 변환
                    system->SetNormalJudgeRange(min * 0.001f, max * 0.001f);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 일격 판정 범위입니다. 단위: ms");
            }

            // Visul
            ImGui::Separator();
            ImGuiHelper::AlignedText("Visual", ImGuiHelper::LEFT, 0.8f);
            {
                float factor = system->GetJudgePosFactor();
                ImGuiHelper::TextWithVerticalSeparator("Perfect Note Pos Factor", labelWidth);
                if (ImGui::DragFloat("##perfect_note_pos_factor", &factor, 0.01f, 0.0f, 1.0f))
                {
                    system->SetJudgePosFactor(factor);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 판정 위치 비율입니다. (0 ~ 1)");

                auto [inMin, inMax] = system->GetFadeInPosFactor();
                ImGuiHelper::TextWithVerticalSeparator("Fade In Pos Factor", labelWidth);
                if (ImGui::DragFloat2("##fade_in_pos_factor", &inMin, 0.01f, -1.0f, 1.0f))
                {
                    system->SetFadeInPosFactor(inMin, inMax);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 페이드인 위치 비율입니다. (0 ~ 1)");
                auto [outMin, outMax] = system->GetFadeOutPosFactor();
                ImGuiHelper::TextWithVerticalSeparator("Fade Out Pos Factor", labelWidth);
                if (ImGui::DragFloat2("##fade_out_pos_factor", &outMin, 0.01f, -1.0f, 1.0f))
                {
                    system->SetFadeOutPosFactor(outMin, outMax);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 페이드아웃 위치 비율입니다. (-1 ~ 0)");
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

            // Weapon ID
            int weaponID = _qteTrack.GetWeaponID();
            ImGuiHelper::TextWithVerticalSeparator("Weapon ID", labelWidth);
            WeaponTableComponent* weaponTable = WeaponTableComponent::GetInstance();
            if (weaponTable)
            {
                auto& table = weaponTable->GetWeaponTable();
                if (ImGui::BeginCombo("##weapon_id", std::to_string(weaponID).c_str()))
                {
                    for (const auto& [name, weapon] : table)
                    {
                        bool isSelected = (weaponID == weapon.Stats.WeaponID);
                        const std::string& label = weapon.Stats.WeaponName;
                        if (ImGui::Selectable(label.c_str(), isSelected))
                        {
                            _qteTrack.SetWeaponID(weapon.Stats.WeaponID);
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            // MinFrame
            float minFrame = track->GetMinFrame();
            ImGuiHelper::TextWithVerticalSeparator("Min Frame", labelWidth);
            if (ImGui::DragFloat("##min_frame", &minFrame, 0.1f))
            {
                track->SetMinFrame(minFrame);
            }

            // MaxFrame
            float maxFrame = track->GetMaxFrame();
            ImGuiHelper::TextWithVerticalSeparator("Max Frame", labelWidth);
            if (ImGui::DragFloat("##max_frame", &maxFrame, 0.1f))
            {
                track->SetMaxFrame(maxFrame);
            }

            // Speed Scale 
            float speed = _qteTrack.GetQTESpeedScale();
            ImGuiHelper::TextWithVerticalSeparator("Speed Scale", labelWidth);
            if (ImGui::DragFloat("##speed_scale_frame", &speed, 0.01f))
            {
                _qteTrack.SetQTESpeedScale(speed);
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
                if (ImGui::BeginTabItem("Note Edit"))
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
                if (ImGui::BeginTabItem("dummy tab"))
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
        float  delayTime        = system->GetDelayFromQTEStart();

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
            _previewTimer     = minFrame;
            _delayTimer       = 0.0f;
            _isPreviewPlaying = false;
            track->SetCurrentFrame(_previewTimer, true);
            _noteJudgeSet.clear();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Stop");

        ImGui::PopStyleVar(); // 라운딩 원복

        // 타이머
        ImGui::SameLine();
        if (_delayTimer <= delayTime)
        {
            ImGui::Text("Delay Time: %.2f s", _delayTimer);
        }
        else
        {
            ImGui::Text("Time: %.2f s", _previewTimer);
        }

        DrawPreview();
        if (_isPreviewPlaying)
        {
            _sequencerEditor.RemoveFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
            if (_delayTimer <= delayTime)
            {
                _delayTimer += ImGui::GetIO().DeltaTime;
            }
            else
            {
                _previewTimer += ImGui::GetIO().DeltaTime * trackSpeedScale * globalSpeedScale;
            }
            track->SetCurrentFrame(_previewTimer, true);
            if (_previewTimer >= maxFrame)
            {
                _isPreviewPlaying = false;
                _delayTimer       = 0.0f;
                _previewTimer     = minFrame;
                track->SetCurrentFrame(_previewTimer, true);
                _noteJudgeSet.clear();
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
        auto system = QTESystem::GetInstance();
        auto track  = _qteTrack.GetEventTrack().lock();
        if (system && track)
        {
            float                   circleRadius = 30.0f;
            auto*                   drawList     = window->DrawList;
            ImVec2                  offset       = ImGui::GetCursorScreenPos();
            ImVec2                  availSize    = ImGui::GetContentRegionAvail();
            std::pair<float, float> perfectRange = system->GetPerfectJudgeRange();
            std::pair<float, float> normalRange  = system->GetNormalJudgeRange();

            float bgAlpha = (_PerfectTimer / PERFECT_EFFECT_TIME);
            drawList->AddRectFilled(offset, offset + availSize, ImColor(0.2f, 0.2f, 0.2f, 1.0f));

            DrawJudgeRange(normalRange, circleRadius, ImColor(100, 255, 100, 255),
                           ImColor(0.3f, 0.3f, 0.3f, 0.5f + 0.5f * bgAlpha));
            DrawJudgeRange(perfectRange, circleRadius, ImColor(140, 120, 170, 255));

            if (_PerfectTimer > 0.0f)
            {
                _PerfectTimer -= ImGui::GetIO().DeltaTime;
                if (_PerfectTimer < 0.0f)
                {
                    _PerfectTimer = 0.0f;
                }
            }
            for (const auto& note : track->GetEventContextQueue())
            {
                DrawNote(note, circleRadius, ImColor(100, 100, 255, 255), ImColor(100, 100, 255, 100));
            }
        }
    }

    ImGui::EndChild();
}

void QTEEditor::DrawJudgeRange(std::pair<float, float> range, float circleRadius, ImU32 judgeCol, ImU32 bgCol)
{
    auto* window = ImGui::GetCurrentWindow();
    auto* system = QTESystem::GetInstance();
    if (system && window && window->DrawList)
    {
        auto* drawList = window->DrawList;

        ImVec2 offset          = ImGui::GetCursorScreenPos();
        ImVec2 availSize       = ImGui::GetContentRegionAvail();
        float  centerPosFactor = system->GetJudgePosFactor();

        auto& [min, max] = range;
        float centerPosX = availSize.x * centerPosFactor;
        float minPosX    = centerPosX + (availSize.x * min);
        float maxPosX    = centerPosX + (availSize.x * max);

        if (bgCol != UINT_MAX - 1)
        {
            drawList->AddRectFilled(offset + ImVec2(minPosX - circleRadius, 0.0f),
                                    offset + ImVec2(maxPosX + circleRadius, availSize.y), bgCol);
        }

        drawList->AddCircleFilled(offset + ImVec2(minPosX, availSize.y * 0.5f), circleRadius, judgeCol);
        drawList->AddCircleFilled(offset + ImVec2(maxPosX, availSize.y * 0.5f), circleRadius, judgeCol);
        drawList->AddRectFilled(offset + ImVec2(minPosX, availSize.y * 0.5f - circleRadius),
                                offset + ImVec2(maxPosX, availSize.y * 0.5f + circleRadius), judgeCol);
    }
}

void QTEEditor::DrawNote(Timeline::EventContext* context, float circleRadius, ImColor noteCol, ImColor bgCol)
{
    auto* window = ImGui::GetCurrentWindow();
    auto* system = QTESystem::GetInstance();
    if (system && window && window->DrawList)
    {
        auto*  drawList        = window->DrawList;
        ImVec2 offset          = ImGui::GetCursorScreenPos();
        ImVec2 availSize       = ImGui::GetContentRegionAvail();
        float  centerPosFactor = system->GetJudgePosFactor();
        float  centerPosX      = availSize.x * centerPosFactor;
        float  noteTime        = context->Time;
        float  posX            = (1.0f + _previewTimer - noteTime) * centerPosX;
        if (posX > availSize.x)
        {
            return;
        }
        float alphaFactor = CalcNoteAlphaFromPositionX(posX);
        noteCol.Value.w *= alphaFactor;
        bgCol.Value.w *= alphaFactor;

        ImRect noteBgRect =
            ImRect(offset + ImVec2(posX - circleRadius, 0.0f), offset + ImVec2(posX + circleRadius, availSize.y));
        drawList->AddRectFilled(noteBgRect.Min, noteBgRect.Max, bgCol);
        drawList->AddCircleFilled(offset + ImVec2(posX, availSize.y * 0.5f), circleRadius, noteCol);

        auto [min, max] = system->GetPerfectJudgeRange();
        float minPosX   = centerPosX + (availSize.x * min);
        float maxPosX   = centerPosX + (availSize.x * max);
        if (posX >= minPosX)
        {
            if (_noteJudgeSet.find(context->ID) == _noteJudgeSet.end())
            {
                _noteJudgeSet.insert(context->ID);
                _PerfectTimer = PERFECT_EFFECT_TIME;
            }
        }
    }
}

float QTEEditor::CalcNoteAlphaFromPositionX(float posX)
{
    auto system = QTESystem::GetInstance();
    if (system)
    {
        ImVec2 availSize              = ImGui::GetContentRegionAvail();
        auto [fadeInMin, fadeInMax]   = system->GetFadeInPosFactor();
        auto [fadeOutMin, fadeOutMax] = system->GetFadeOutPosFactor();

        float alpha = 1.0f;

        float inStart  = availSize.x * fadeInMin;
        float inEnd    = availSize.x * fadeInMax;
        float outStart = availSize.x * fadeOutMin;
        float outEnd   = availSize.x * fadeOutMax;

        if (posX < inStart)
        {
            alpha = 0.0f; // 아직 페이드인 시작 전
        }
        else if (posX < inEnd)
        {
            float t = (posX - inStart) / (inEnd - inStart);
            alpha   = t; // 0 → 1 보간
        }
        else if (posX > outEnd)
        {
            alpha = 0.0f; // 이미 페이드아웃 끝남
        }
        else if (posX > outStart)
        {
            float t = (posX - outStart) / (outEnd - outStart);
            alpha   = 1.0f - t; // 1 → 0 보간
        }

        return alpha;
    }
    return 1.0f;
}