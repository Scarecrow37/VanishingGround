#include "pchScripts.h"
#include "QTEEditor.h"
#include "Utility/SingletonHelper.h"
#include <QTE/System/QTESystem.h>
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
    _sequencerEditor.SetFlags(flags);
    
    auto& callback = _sequencerEditor.GetCallback();
    callback.LowerFramePopup = [this](Timeline::EventTrack& track) {
        if (ImGui::MenuItem("Paste Note"))
        {
            float time = _sequencerEditor.GetFrameFromIndicate();
            track.PasteContext(_copyBuffer, time);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Add Note"))
        {
            float min, max, frame;
            min   = track.GetMinFrame();
            max   = track.GetMaxFrame();
            frame = _sequencerEditor.GetFrameFromIndicate();
            track.AddEvent<QTE::Note>("Note", ImClamp(frame, min, max));
        }
    };

    callback.ContextPopup = [this](Timeline::EventTrack& track, Timeline::EventContext& context) {
        if (ImGui::MenuItem("Copy Note"))
        {
            _copyBuffer = track.CopyContext(&context);
        }
    };

    _lastUsedPath = UmFileSystem.GetRootPath();

    _sequencerEditor.SetEventTrack(_editTrack.GetEventTrack());
}

QTEEditor::~QTEEditor() 
{
    _previewTrack = nullptr;
}

void QTEEditor::Show()
{
    if (_editorOpened)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
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

        ProcessInputEvent();

        ImGui::End();
    }
}

void QTEEditor::Open() 
{
    _previewTrack = nullptr;
    _editorOpened = true;
}

bool QTEEditor::NewFileWithDialog(QTE::Track* qteTrack)
{
    if (qteTrack == nullptr)
    {
        return false;
    }
    if (_editTrack.IsLoadedFile())
    {
        int result = MessageBox(UmApplication.GetHwnd(), L"기존 파일을 저장하시겠습니까?",
                                L"주의", MB_YESNO);
        if (result == IDYES)
        {
            _editTrack.SaveFile(File::NULL_PATH, true);
        }
    }

    File::Path out;
    LPCWSTR    title       = L"QTE 파일 만들기";
    LPCWSTR    initialDir  = L"";
    LPCWSTR    defaultName = QTE::Track::DEFAULT_NAME.c_str();

    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), title, initialDir, defaultName, {}, out))
    {
        return qteTrack->NewFile(out);
    }
    return false;
}

bool QTEEditor::LoadFileWithDialog(QTE::Track* qteTrack)
{
    if (qteTrack == nullptr)
    {
        return false;
    }
    std::vector<File::Path>                  out;
    HWND                                     owner   = UmApplication.GetHwnd();
    LPCWSTR                                  title   = L"QTE 파일 열기";
    std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"QTE 트랙 파일\0", L"*.UmQTETrack\0"},
                                                        {L"All File\0", L"*.*\0"}};
    if (File::ShowOpenFileDialog(owner, title, _lastUsedPath.c_str(), filters, false, out))
    {
        _lastUsedPath = out.front().parent_path();
        return qteTrack->LoadFile(out.front());
    }
    return false;
}

bool QTEEditor::SaveFileWithDialog(QTE::Track* qteTrack)
{
    if (qteTrack == nullptr)
    {
        return false;
    }
    File::Path out;
    HWND       owner       = UmApplication.GetHwnd();
    LPCWSTR    title       = L"QTE 파일 저장";
    LPCWSTR    defaultName = QTE::Track::DEFAULT_NAME.c_str();
    if (File::ShowSaveFileDialog(owner, title, _lastUsedPath.c_str(), defaultName, {}, out))
    {
        _lastUsedPath = out.parent_path();
        return qteTrack->SaveFile(out);
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
        QTESystem* system = SingletonComponent<QTESystem>::GetInstance();
        if (system)
        {
            ImGui::BeginChild("system_props", ImVec2(0.0f, ImGui::GetContentRegionAvail().y * 0.4f), ImGuiChildFlags_Border);

            float labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;
            ImGuiHelper::AlignedText("Properties", ImGuiHelper::LEFT, 0.8f);
            // 스피드
            float speed = system->GetQTESpeedScale();
            ImGuiHelper::TextWithVerticalSeparator("QTE Speed Scale", labelWidth);
            if (ImGui::DragFloat("##qte_speed", &speed, 0.01f, 0.1f, 10.0f))
            {
                system->SetQTESpeedScale(speed);
            }
            ImGuiHelper::HoveredToolTip((const char*)u8"QTE 글로벌 속도 배율입니다.");

            // 스피드
            float travel = system->GetNoteTravelTime();
            ImGuiHelper::TextWithVerticalSeparator("QTE Note Travel Time", labelWidth);
            if (ImGui::DragFloat("##qte_note_tavel", &travel, 0.01f, 0.1f, 10.0f))
            {
                system->SetNoteTravelTime(travel);
            }
            ImGuiHelper::HoveredToolTip((const char*)u8"QTE 노트가 퍼펙트까지 걸리는 시간입니다.");

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

            // 유효 판정 범위
            {
                auto [min, max] = system->GetValidJudgeRange();
                // ms 단위로 변환
                min *= 1000.0f;
                max *= 1000.0f;
                ImGuiHelper::TextWithVerticalSeparator("QTE Valid Judge Range", labelWidth);
                if (ImGui::DragFloat2("##qte_valid_range", &min, 0.01f))
                {
                    // s 단위로 변환
                    system->SetValidJudgeRange(min * 0.001f, max * 0.001f);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 유효 판정 범위입니다. 단위: ms");
            }

            // Visul
            ImGui::Separator();
            ImGuiHelper::AlignedText("Visual", ImGuiHelper::LEFT, 0.8f);
            {
                auto [inMin, inMax] = system->GetFadeInPosFactor();
                auto [outMin, outMax] = system->GetFadeOutPosFactor();
                ImGuiHelper::TextWithVerticalSeparator("Fade In Pos Factor", labelWidth);
                if (ImGui::SliderFloat2("##fade_in_pos_factor", &inMin, 0.0f, 1.0f))
                {
                    auto [oldMin, oldMax] = system->GetFadeInPosFactor();
                    // Min조작 = Max를 변경
                    if (inMin != oldMin)
                    {
                        inMin = ImClamp(inMin, 0.0f, 1.0f);
                        inMax = ImClamp(inMax, inMin, 1.0f);
                    }
                    // Max조작 = Min를 변경
                    else if (inMax != oldMax)
                    {
                        inMax = ImClamp(inMax, 0.0f, 1.0f);
                        inMin = ImClamp(inMin, 0.0f, inMax);
                    }
                    outMin = ImClamp(outMin, inMax, outMax);
                    outMax = ImClamp(outMax, outMin, 1.0f);
                    system->SetFadeInPosFactor(inMin, inMax);
                    system->SetFadeOutPosFactor(outMin, outMax);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 페이드인 위치 비율입니다. (0 ~ 1)");
                ImGuiHelper::TextWithVerticalSeparator("Fade Out Pos Factor", labelWidth);
                if (ImGui::SliderFloat2("##fade_out_pos_factor", &outMin, 0.0f, 1.0f))
                {
                    auto [oldMin, oldMax] = system->GetFadeOutPosFactor();
                    // Min조작 = Max를 변경
                    if (outMin != oldMin)
                    {
                        outMin = ImClamp(outMin, 0.0f, 1.0f);
                        outMax = ImClamp(outMax, outMin, 1.0f);
                    }
                    // Max조작 = Min를 변경
                    else if (outMax != oldMax)
                    {
                        outMax = ImClamp(outMax, 0.0f, 1.0f);
                        outMin = ImClamp(outMin, 0.0f, outMax);
                    }
                    inMin = ImClamp(inMin, 0.0f, inMax);
                    inMax = ImClamp(inMax, inMin, outMin);
                    system->SetFadeInPosFactor(inMin, inMax);
                    system->SetFadeOutPosFactor(outMin, outMax);
                }
                ImGuiHelper::HoveredToolTip((const char*)u8"QTE 페이드아웃 위치 비율입니다. (0 ~ 1)");
            }
            ImGui::EndChild();
            ImGui::BeginChild("system_tracks", ImVec2(0, 0), ImGuiChildFlags_Border);

            // 무기 별 트랙 정보
            ImGuiHelper::AlignedText("Track", ImGuiHelper::LEFT, 0.8f);
            auto weaponTableComponent = SingletonComponent<WeaponTableComponent>::GetInstance();
            if (weaponTableComponent)
            {
                const auto& trackTable = system->GetWeaponIDToTrackTable();
                const auto& weaponTable = weaponTableComponent->GetWeaponTable();

                if (ImGui::Button("Reset Track"))
                {
                    system->ClearTrack();
                }
                if (ImGui::Button("Load Track"))
                {
                    std::vector<File::Path>                  out;
                    HWND                                     owner   = UmApplication.GetHwnd();
                    LPCWSTR                                  title   = L"QTE 파일 열기";
                    std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"QTE 트랙 파일\0", L"*.UmQTETrack\0"},
                                                                        {L"All File\0", L"*.*\0"}};
                    if (File::ShowOpenFileDialog(owner, title, _lastUsedPath.c_str(), filters, true, out))
                    {
                        for (const File::Path& path : out)
                        {
                            if (path.extension() == QTE::Track::EXTENSION)
                            {
                                size_t      index = 0;
                                std::string idStr = path.stem().string();
                                int         id    = std::stoi(idStr, &index);
                                if (index == idStr.size())
                                {
                                    system->AddMappingTrackToWeaponID(id, path);
                                }
                            }
                        }
                    }
                }
                for (const auto& [weaponName, weapon] : weaponTable)
                {
                    int  weaponID   = weapon.Stats.WeaponID;
                    auto itr        = trackTable.find(weaponID);
                    bool valid      = (itr != trackTable.end() && false == itr->second.empty());

                    ImGuiHelper::StyleBuilder style;
                    ImVec4 textCol = valid ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
                    style.PushStyleColor(ImGuiCol_Text, textCol);

                    if (ImGui::TreeNodeEx(weaponName.c_str()))
                    {
                        ImGui::Text("ID: %d", weaponID);
                        style.PopStyle();
                        int trackCount = 0;
                        if (valid)
                        {
                            auto& trackVector = itr->second;
                            for (const auto& track : trackVector)
                            {
                                ShowTrackFromWeapon(track, weaponName, weaponID, trackCount);
                                ++trackCount;
                            }
                        }
                        else
                        {
                            ImGui::TextDisabled("Invalid QTE Track");
                        }

                        ImGui::Separator();
                        if (ImGui::Selectable("+ Add QTE Track"))
                        {   
                            system->AddMappingTrackToWeaponID(weaponID);
                        }
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::EndChild();
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
    ImGui::BeginChild("track_detail", canvasSize, ImGuiChildFlags_Border);
    {
        if (ImGui::BeginTabBar("##AnimationTrackTabs"))
        {
            if (ImGui::BeginTabItem("preview"))
            {
                ImGui::BeginChild("##preview_tab", ImVec2(0, 0), ImGuiChildFlags_Border);
                // Preview Frame
                ShowPreviewFrame();
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("edit"))
            {
                ImGui::BeginChild("##edit_frame", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_MenuBar);
                // File Menu
                ShowTrackDetailMenu();
                // Edit Frame
                ShowEditFrame();
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}

void QTEEditor::ShowTrackDetailMenu()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Track", EditorIcon::ICON_FILE, false))
            {
                NewFileWithDialog(&_editTrack);
            }
            if (ImGui::MenuItem("Load Track", EditorIcon::ICON_FOLDER_OPEN, false))
            {
                LoadFileWithDialog(&_editTrack);
            }
            if (ImGui::MenuItem("Save Track", EditorIcon::ICON_FILE_SAVE, false, _editTrack.IsLoadedFile()))
            {  
                _editTrack.SaveFile(File::NULL_PATH, true);
            }
            if (ImGui::MenuItem("Save As Track", EditorIcon::ICON_FILE_SAVE, false, _editTrack.IsLoadedFile()))
            {
                SaveFileWithDialog(&_editTrack);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void QTEEditor::ShowEditFrame() 
{
    ImVec2 availSize    = ImGui::GetContentRegionAvail();
    float  labelWidth   = ImClamp(availSize.x * 0.2f, 30.0f, 150.0f);
    ImVec2 size         = ImGui::GetItemRectSize();

    bool isFileLoaded = _editTrack.IsLoadedFile();

    std::string pathBuffer = isFileLoaded ? _editTrack.GetFilePath().string() : "No Track Loaded";
    ImGuiHelper::TextWithVerticalSeparator("Track Path", labelWidth);
    ImGui::SetNextItemWidth(availSize.x * 0.5f);
    ImGui::InputText("##path_input", &pathBuffer, ImGuiInputTextFlags_ReadOnly);
    ImGuiHelper::HoveredToolTip(pathBuffer.c_str());

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FILE, ImVec2(size.y, size.y)))
    {
        NewFileWithDialog(&_editTrack);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("New QTE Track");

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN, ImVec2(size.y, size.y)))
    {
        LoadFileWithDialog(&_editTrack);
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Open QTE Track");

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FILE_SAVE, ImVec2(size.y, size.y)))
    {
        const File::Path& filePath = _editTrack.GetFilePath();
        if (true == filePath.IsNull())
        {
            SaveFileWithDialog(&_editTrack);
        }
        else
        {
            _editTrack.SaveFile(filePath, true);
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Save QTE Track");

    if (false == isFileLoaded)
    {
        ImGui::BeginDisabled();
    }
    auto track = _editTrack.GetEventTrack().lock();
    if (track)
    {
        // Min Frame
        float minFrame = _editTrack.GetMinFrame();
        ImGuiHelper::TextWithVerticalSeparator("Min Frame", labelWidth);
        if (ImGui::DragFloat("##min_frame", &minFrame, 0.1f))
        {
            track->SetMinFrame(minFrame);
        }

        // Max Frame
        float maxFrame = _editTrack.GetMaxFrame();
        ImGuiHelper::TextWithVerticalSeparator("Max Frame", labelWidth);
        if (ImGui::DragFloat("##max_frame", &maxFrame, 0.1f))
        {
            track->SetMaxFrame(maxFrame);
        }

        // Speed Scale
        float speed = _editTrack.GetQTESpeedScale();
        ImGuiHelper::TextWithVerticalSeparator("Speed Scale", labelWidth);
        if (ImGui::DragFloat("##speed_scale_frame", &speed, 0.01f))
        {
            _editTrack.SetQTESpeedScale(speed);
        }

        // Sequencer Canvas
        ShowSequencerFrame(track);

        // Preview Canvas
        ImGui::BeginChild("##preview_canvas", ImVec2(0, 0), ImGuiChildFlags_Border);
        DrawPreview(&_editTrack);
        ImGui::EndChild();
    }
    if (false == isFileLoaded)
    {
        ImGui::EndDisabled();
    }
}

void QTEEditor::ShowSequencerFrame(std::shared_ptr<Timeline::EventTrack> track)
{
    if (track)
    {
        float heightScale = 0.6f;
        // Sequencer
        {
            int flags = Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE |
                        Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_MIN_MAX_LINE |
                        Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CONTEXT |
                        Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU |
                        Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_CONTEXT_MENU |
                        Timeline::SequencerEditor::FLAGS_HIDE_MIN_MAX_LINE |
                        Timeline::SequencerEditor::FLAGS_HIDE_CONTEXT_LABEL |
                        Timeline::SequencerEditor::FLAGS_DRAW_CONTEXT_LINE_VERTICAL;
            _sequencerEditor.SetFlags(flags);
            _sequencerEditor.SetEventTrack(track);
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
            if (ImGui::BeginTabBar("##anim_track_tab"))
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
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
        }
    }
}

void QTEEditor::ShowPreviewFrame() 
{
    if (nullptr == _previewTrack)
    {
        ImGui::TextUnformatted("No Selected QTE Track");
        return;
    }

    auto system = SingletonComponent<QTESystem>::GetInstance();
    auto track  = _previewTrack->GetEventTrack().lock();
    if (system && track)
    {
        float  maxFrame   = track->GetMaxFrame();
        float  minFrame   = track->GetMinFrame();
        ImVec2 availSize  = ImGui::GetContentRegionAvail();
        ImVec2 buttonSize = ImGui::GetItemRectSize();
        float  spacing    = ImGui::GetStyle().ItemSpacing.x;
        float  delayTime  = system->GetDelayFromQTEStart();
        float  labelWidth = ImClamp(availSize.x * 0.2f, 30.0f, 150.0f);

        bool isFileLoaded = _previewTrack->IsLoadedFile();

        // File Path
        std::string pathBuffer = isFileLoaded ? _previewTrack->GetFilePath().string() : "No Set QTE Track";
        ImGuiHelper::TextWithVerticalSeparator("Track Path", labelWidth);
        ImGui::SetNextItemWidth(availSize.x * 0.5f);
        ImGui::InputText("##path_input", &pathBuffer, ImGuiInputTextFlags_ReadOnly);
        ImGuiHelper::HoveredToolTip(pathBuffer.c_str());

        ImGui::SameLine();
        if (ImGui::Button(EditorIcon::ICON_FILE, ImVec2(buttonSize.y, buttonSize.y)))
        {
            LoadFileWithDialog(_previewTrack);
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Set QTE Track");

        ImGui::SameLine();
        if (ImGui::Button(EditorIcon::ICON_CIRCLE, ImVec2(buttonSize.y, buttonSize.y)))
        {
            _previewTrack->LoadFile(_previewTrack->GetFilePath());
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Refesh");

        // Sequencer Canvas
        {
            int flags = Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE |
                        Timeline::SequencerEditor::FLAGS_HIDE_MIN_MAX_LINE |
                        Timeline::SequencerEditor::FLAGS_HIDE_CONTEXT_LABEL |
                        Timeline::SequencerEditor::FLAGS_DRAW_CONTEXT_LINE_VERTICAL;
            _sequencerEditor.SetFlags(flags);
            _sequencerEditor.SetEventTrack(track);

            ImVec2 availSize  = ImGui::GetContentRegionAvail();
            ImVec2 canvasSize = ImVec2(availSize.x, availSize.y * 0.6f);
            ImGui::BeginChild("sequencer_frame", canvasSize, ImGuiChildFlags_Border, ImGuiWindowFlags_NoScrollWithMouse);
            _sequencerEditor.Show();
            ImGui::EndChild();
        }

        // Preview Canvas
        ImGui::BeginChild("##preview_canvas", ImVec2(0, 0), ImGuiChildFlags_Border);
        DrawPreview(_previewTrack);
        ImGui::EndChild();
    }
}

void QTEEditor::ShowTrackFromWeapon(const QTE::Track* track, const std::string& weaponName, int weaponID, int index) 
{
    auto system   = SingletonComponent<QTESystem>::GetInstance();
    bool selected = (_previewTrack == track);
    if (nullptr == system || nullptr == track)
    {
        return;
    }
    std::string label = std::format("Track {}", index + 1);
    if (ImGui::Selectable(label.c_str(), selected))
    {
        QTE::Track* track = system->GetMappingTrackToWeaponID(weaponID, index);
        if (track)
        {
            _previewTrack = track;
        }
    }
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::Selectable("- Delete QTE Track"))
        {
            std::wstring msg   = L"QTE 트랙을 삭제하시겠습니까?";
            std::wstring title = L"Warning";
            HWND         hwnd  = UmApplication.GetHwnd();

            int msgResult = MessageBox(hwnd, msg.c_str(), title.c_str(), MB_YESNO);
            if (msgResult == IDYES)
            {
                if (system->RemoveMappingTrackToWeaponID(weaponID, index))
                {
                    _previewTrack = nullptr;
                    _sequencerEditor.SetEventTrack(std::weak_ptr<Timeline::EventTrack>());
                }
            }
        }
        ImGui::EndPopup();
    }
}

void QTEEditor::ProcessInputEvent() 
{
    auto        track    = _editTrack.GetEventTrack().lock();
    UINT        id       = _sequencerEditor.GetSelectedContextID();
    const float minFrame = _editTrack.GetMinFrame();
    const float maxFrame = _editTrack.GetMaxFrame();
    if (track)
    {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
        {

            if (ImGui::IsKeyPressed(ImGuiKey_C))
            {
                _copyBuffer = track->CopyContextFromID(id);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_V))
            {
                float time = _sequencerEditor.GetFrameFromMousePos();
                track->PasteContext(_copyBuffer, time);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
            {
                if (Timeline::EventContext* context = track->GetContextFromID(id))
                {
                    float newTime = ImClamp(context->Time + 0.1f, minFrame, maxFrame);
                    track->ChangeContextTime(id, newTime);
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
            {
                if (Timeline::EventContext* context = track->GetContextFromID(id))
                {
                    float newTime = ImClamp(context->Time - 0.1f, minFrame, maxFrame);
                    track->ChangeContextTime(id, newTime);
                }
            }
        }
        else
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
            {
                Timeline::EventContext* context;
                UINT                    nextId = 0;
                if (context = track->GetPrevContextFromID(id))
                {
                    nextId = context->ID;
                }
                else if (context = track->GetNextContextFromID(id))
                {
                    nextId = context->ID;
                }
                track->RemoveContextFromID(id);
                _sequencerEditor.SetSelectedContextID(nextId);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow) || ImGui::IsKeyPressed(ImGuiKey_UpArrow))
            {
                if (Timeline::EventContext* context = track->GetNextContextFromID(id))
                {
                    UINT nextId = context->ID;
                    _sequencerEditor.SetSelectedContextID(nextId);
                    _sequencerEditor.SetViewPositionFromID(nextId, Timeline::SequencerEditor::ALIGN_CENTER);
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) || ImGui::IsKeyPressed(ImGuiKey_DownArrow))
            {
                if (Timeline::EventContext* context = track->GetPrevContextFromID(id))
                {
                    UINT prevId = context->ID;
                    _sequencerEditor.SetSelectedContextID(prevId);
                    _sequencerEditor.SetViewPositionFromID(prevId, Timeline::SequencerEditor::ALIGN_CENTER);
                }
            }
        }
    }
}

void QTEEditor::DrawPreview(QTE::Track* qteTrack)
{
    ImVec2 buttonSize = ImVec2(ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().y); // 정사각형 버튼

    auto* window = ImGui::GetCurrentWindow();
    if (qteTrack && window && window->DrawList)
    {
        auto system = SingletonComponent<QTESystem>::GetInstance();
        auto track  = qteTrack->GetEventTrack().lock();
        if (system && track)
        {
            float maxFrame = track->GetMaxFrame();
            float minFrame = track->GetMinFrame();
            float delayTime = system->GetDelayFromQTEStart();

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

            if (ImGui::Button(EditorIcon::ICON_PLAY, buttonSize))
            {
                _isPreviewPlaying = true;
                _previewTimer     = minFrame - delayTime;
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
                _isPreviewPlaying = false;
                _previewTimer     = minFrame - delayTime;
                track->SetCurrentFrame(_previewTimer, true);
                _noteJudgeSet.clear();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Stop");

            ImGui::PopStyleVar(); // 라운딩 원복

            // 타이머
            ImGui::SameLine();
            ImGui::Text("Time: %.2f s", _previewTimer);

            float                   circleRadius = 15.0f;
            auto*                   drawList     = window->DrawList;
            ImVec2                  offset       = ImGui::GetCursorScreenPos();
            ImVec2                  availSize    = ImGui::GetContentRegionAvail();
            std::pair<float, float> perfectRange = system->GetPerfectJudgeRange();
            std::pair<float, float> normalRange  = system->GetNormalJudgeRange();
            std::pair<float, float> validRange   = system->GetValidJudgeRange();

            float bgAlpha = (_perfectTimer / PERFECT_EFFECT_TIME);
            drawList->AddRectFilled(offset, offset + availSize, ImColor(0.2f, 0.2f, 0.2f, 1.0f));

            DrawJudgeRange(qteTrack, validRange, ImColor(0.3f, 0.3f, 0.3f, 1.0f), ImColor(0.3f, 0.3f, 0.3f, 0.5f + 0.5f * bgAlpha));
            DrawJudgeRange(qteTrack, normalRange, ImColor(100, 255, 100, 255));
            DrawJudgeRange(qteTrack, perfectRange, ImColor(140, 120, 170, 255));

            if (_perfectTimer > 0.0f)
            {
                _perfectTimer -= ImGui::GetIO().DeltaTime;
                if (_perfectTimer < 0.0f)
                {
                    _perfectTimer = 0.0f;
                }
            }
            for (const auto& note : track->GetEventContextQueue())
            {
                float thickness = 3.0f;
                DrawNote(qteTrack, note, thickness, ImColor(100, 100, 255, 255), ImColor(100, 100, 255, 100));
            }
            if (_isPreviewPlaying)
            {
                _sequencerEditor.RemoveFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
                _previewTimer += ImGui::GetIO().DeltaTime;
                track->SetCurrentFrame(_previewTimer, true);
                if (_previewTimer >= maxFrame)
                {
                    _isPreviewPlaying = false;
                    _previewTimer     = minFrame - delayTime;
                    track->SetCurrentFrame(_previewTimer, true);
                    _noteJudgeSet.clear();
                }
            }
            else
            {
                _sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
                _previewTimer = track->GetCurrentFrame();
            }
        }
    }
}

void QTEEditor::DrawJudgeRange(QTE::Track* qteTrack, std::pair<float, float> range, ImU32 judgeCol, ImU32 bgCol)
{
    auto* window = ImGui::GetCurrentWindow();
    auto* system = SingletonComponent<QTESystem>::GetInstance();
    if (system && window)
    {
        auto* drawList = window->DrawList;
        if (drawList && qteTrack)
        {
            ImVec2 offset           = ImGui::GetCursorScreenPos();
            ImVec2 availSize        = ImGui::GetContentRegionAvail();
            float  centerPosFactor  = 0.8f;
            float  systemSpeed      = system->GetQTESpeedScale();
            float  trackSpeed       = qteTrack->GetQTESpeedScale();
            auto& [min, max]        = range;
            float centerPosX        = availSize.x * centerPosFactor;
            float minPosX           = centerPosX * (1.0f + min * systemSpeed * trackSpeed);
            float maxPosX           = centerPosX * (1.0f + max * systemSpeed * trackSpeed);

            if (bgCol != UINT_MAX - 1)
            {
                drawList->AddRectFilled(offset + ImVec2(minPosX, 0.0f),
                                        offset + ImVec2(maxPosX, availSize.y), bgCol);
            }

            //drawList->AddCircleFilled(offset + ImVec2(minPosX, availSize.y * 0.5f), circleRadius, judgeCol);
            //drawList->AddCircleFilled(offset + ImVec2(maxPosX, availSize.y * 0.5f), circleRadius, judgeCol);
            drawList->AddRectFilled(offset + ImVec2(minPosX, availSize.y * 0.4f),
                                    offset + ImVec2(maxPosX, availSize.y * 0.6f), judgeCol);
        }
    }
}

void QTEEditor::DrawNote(QTE::Track* qteTrack, Timeline::EventContext* context, float circleRadius, ImColor noteCol,
                         ImColor bgCol)
{
    auto* window = ImGui::GetCurrentWindow();
    auto* system = SingletonComponent<QTESystem>::GetInstance();
    if (system && window)
    {
        auto* drawList = window->DrawList;
        if (drawList && context && qteTrack)
        {
            ImVec2 offset           = ImGui::GetCursorScreenPos();
            ImVec2 availSize        = ImGui::GetContentRegionAvail();
            float  systemSpeed      = system->GetQTESpeedScale();
            float  trackSpeed       = qteTrack->GetQTESpeedScale();
            float  timer            = _previewTimer * systemSpeed * trackSpeed;
            float  noteTime         = context->Time * systemSpeed * trackSpeed;
            float  centerPosFactor  = 0.8f;
            float  centerPosX       = availSize.x * centerPosFactor;
            float  posX             = centerPosX * (1.0f + (timer - noteTime));
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
            //drawList->AddCircleFilled(offset + ImVec2(posX, availSize.y * 0.5f), circleRadius, noteCol);

            auto [min, max] = system->GetPerfectJudgeRange();
            float minPosX   = centerPosX + (availSize.x * min);
            float maxPosX   = centerPosX + (availSize.x * max);
            if (posX >= minPosX)
            {
                if (_noteJudgeSet.find(context->ID) == _noteJudgeSet.end())
                {
                    _noteJudgeSet.insert(context->ID);
                    _perfectTimer = PERFECT_EFFECT_TIME;
                }
            }
        }
    }
}

float QTEEditor::CalcNoteAlphaFromPositionX(float posX)
{
    auto system = SingletonComponent<QTESystem>::GetInstance();
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