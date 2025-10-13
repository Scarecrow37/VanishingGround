#include "pch.h"
#include "EditorAnimationTrackTool.h"
#include "GraphicsEngine/Interface/IAnimator.h"

EditorAnimationTrackTool::EditorAnimationTrackTool()
{
    SetLabel("Sequencer##model");
    SetDockLayout(ImGuiDir_Down);
    SetImGuiWindowFlag(ImGuiWindowFlags_MenuBar);
}

EditorAnimationTrackTool::~EditorAnimationTrackTool()
{
    if (nullptr != _sequencer)
    {
        delete _sequencer;
        _sequencer = nullptr;
    }
}

void EditorAnimationTrackTool::OnTickGui() 
{
    if (nullptr == _modelDetails)
    {
        const auto* dock = GetOwnerDockWindow();
        _modelDetails    = dock->GetGui<EditorModelDetails>();
    }
    while (false == _eventQueue.empty())
    {
        _eventQueue.front()();
        _eventQueue.pop();
    }
}

void EditorAnimationTrackTool::OnStartGui() 
{
    const auto* dock = GetOwnerDockWindow();
    _modelDetails    = dock->GetGui<EditorModelDetails>();
    _sequencer = new Timeline::SequencerEditor();
    _sequencer->AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_ALL_INPUT);
    _sequencer->GetCallback().LowerFramePopup = [this](Timeline::EventTrack& track) {
        LowerFramePopup(track); 
    };
    _sequencer->GetCallback().ContextPopup    = [this](Timeline::EventTrack& track, Timeline::EventContext& context) {
        ContextPopup(track, context);
    };
    _sequencer->GetCallback().ContextDoubleClick = [this](Timeline::EventTrack& track, Timeline::EventContext& context) {
        FocusDetailTabItem(1);
    };
}

void EditorAnimationTrackTool::OnEndGui() {}

void EditorAnimationTrackTool::OnPreFrameBegin() {}

void EditorAnimationTrackTool::OnPostFrameBegin() {}

void EditorAnimationTrackTool::OnFrameRender() 
{
    UpdateTimeline();

    DrawMenuBar();
    if (_sequencer)
    {
        DrawTimelines();
        ImGui::SameLine();
        DrawCanvas();
        ImGui::SameLine();
        DrawDetails();
    }
}

void EditorAnimationTrackTool::OnFrameEnd() {}

void EditorAnimationTrackTool::OnFrameFocusEnter() {}

void EditorAnimationTrackTool::OnFrameFocusStay() 
{
    auto track = _animationEventTrack.GetActiveEventTrack();
    if (track && _sequencer)
    {
        UINT id = _sequencer->GetSelectedContextID();
        const float minFrame = track->GetMinFrame();
        const float maxFrame = track->GetMaxFrame();
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
        {

            if (ImGui::IsKeyPressed(ImGuiKey_C))
            {
                _copyBuffer = track->CopyContextFromID(id);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_V))
            {
                float time = _sequencer->GetFrameFromIndicate();
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
                _sequencer->SetSelectedContextID(nextId);
            }
            if (ImGui::IsKeyPressed(ImGuiKey_RightArrow) || ImGui::IsKeyPressed(ImGuiKey_UpArrow))
            {
                if (Timeline::EventContext* context = track->GetNextContextFromID(id))
                {
                    UINT nextId = context->ID;
                    _sequencer->SetSelectedContextID(nextId);
                    _sequencer->SetViewPositionFromID(nextId, Timeline::SequencerEditor::ALIGN_CENTER);
                }
            }
            if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) || ImGui::IsKeyPressed(ImGuiKey_DownArrow))
            {
                if (Timeline::EventContext* context = track->GetPrevContextFromID(id))
                {
                    UINT prevId = context->ID;
                    _sequencer->SetSelectedContextID(prevId);
                    _sequencer->SetViewPositionFromID(prevId, Timeline::SequencerEditor::ALIGN_CENTER);
                }
            }
        }
    }
}

void EditorAnimationTrackTool::OnFrameFocusExit() {}

void EditorAnimationTrackTool::OnFramePopupOpened() {}

void EditorAnimationTrackTool::SerializedReflectEvent() {}

void EditorAnimationTrackTool::DeserializedReflectEvent() {}

void EditorAnimationTrackTool::UpdateTimeline() 
{
    auto curDetailAnim = _modelDetails->GetCurrentAnimationName();
    auto curTrackAnim = _animationEventTrack.GetActiveEventTrackName();

    bool isSameAnim = !curDetailAnim.empty() && !curTrackAnim.empty() && (curDetailAnim == curTrackAnim);

    auto model    = _modelDetails->GetModel();
    auto animator = _modelDetails->GetAnimator();
    if (true == isSameAnim)
    {
        if (model && animator)
        {
            auto eventTrack = _animationEventTrack.GetActiveEventTrack();
            if (nullptr != eventTrack)
            {
                eventTrack->AddFlags(Timeline::EVENT_TRCK_FLAGS_NOTIFY_DISABLED);
                eventTrack->SetMinFrame(0.0f);
                eventTrack->SetMaxFrame(animator->GetCurrentAnimationLastTime());
                eventTrack->SetCurrentFrame(animator->GetCurrentAnimationPlayTime());
                _sequencer->RemoveFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
                eventTrack->Update();
            }
        }
    }
    else
    {
        _sequencer->AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
    }
}

void EditorAnimationTrackTool::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New File"))
            {
                NewFileWithDialog();
            }
            if (ImGui::MenuItem("Open File"))
            {
                LoadFileWithDialog();
            }
            if (ImGui::MenuItem("Save File"))
            {
                const File::Path& filePath = _animationEventTrack.GetFilePath();
                if (true == filePath.IsNull())
                {
                    SaveFileWithDialog();
                }
                else
                {
                    _animationEventTrack.SaveFile(filePath, true);
                }
            }
            ImGui::EndMenu();
        }
        if (nullptr != _sequencer)
        {
            bool useSnap = _sequencer->HasFlags(Timeline::SequencerEditor::FLAGS_USE_SNAP_MODE);
            ImVec4 trueColor  = ImVec4(0.1f, 0.2f, 0.21f, 0.8f);
            ImVec4 falseColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
            if (ImGuiHelper::ToggleButton("Snap", &useSnap, trueColor, falseColor))
            {
                _sequencer->ToggleFlags(Timeline::SequencerEditor::FLAGS_USE_SNAP_MODE);
            }
        }
        
        ImGui::EndMenuBar();
    }
}

void EditorAnimationTrackTool::DrawTimelines() 
{
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(200.0f, availSize.y);
    ImGui::BeginChild("SequencerEventTracks", canvasSize, true);

    if (true == IsLoadAsset())
    {
        const auto& table = _animationEventTrack.GetEventTrackTable();
        ImGui::Text("Event Count: %d", table.size());
        ImGui::Separator();

        auto model = _modelDetails->GetModel();
        if (nullptr != model)
        {
            auto animation = model->GetAnimation();
            if (nullptr != animation)
            {
                const auto& animations = animation->GetAnimations();
                for (auto& anim : animations)
                {
                    auto& curDetailAnim = GetCurrentDetailAnimName();
                    auto& curTrackAnim  = GetCurrentEventTrackmName();
                    bool  isPlaying     = !curDetailAnim.empty() && curDetailAnim == anim;
                    bool  isSeleted     = !curTrackAnim.empty() && curTrackAnim == anim;

                    ImGui::PushID(anim);
                    // Check timeline to anim in set
                    auto track = _animationEventTrack.GetEventTrack(anim);
                    std::string           selectableLabel;
                    ImU32                 textColor;
                    if (nullptr != track)
                    {
                        textColor = isPlaying ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 255, 255, 255);
                        selectableLabel = std::format("{}  {}", EditorIcon::ICON_BELL_ON, anim);
                    }
                    else
                    {
                        textColor = IM_COL32(255, 100, 100, 255);
                        selectableLabel = std::format("{}  {}", EditorIcon::ICON_BELL_OFF, anim);
                    }
                    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                    if (ImGui::Selectable(selectableLabel.c_str(), isSeleted))
                    {
                        SetEventTrackFromAnimation(anim);
                        if (_modelDetails)
                        {
                            _modelDetails->ChangeAnimation(anim);
                        }
                    }
                    ImGui::PopStyleColor();

                    if (true == isPlaying)
                    {
                        ImVec2 old = ImGui::GetCursorPos();
                        ImVec2 size = ImGui::GetItemRectSize();
                        ImGui::SameLine(1.5f);
                        ImGuiHelper::LoadingSpinner(size.y * 0.5f, ImColor(100, 255, 100, 255));
                        ImGui::SetCursorPos(old);
                    }

                    // Context
                    if (ImGui::BeginPopupContextItem("EventTrackContextMenu"))
                    {
                        if (nullptr != track)
                        {
                            if (ImGui::MenuItem("Remove Event Track"))
                            {
                                RemoveEventTrackFromAnimation(anim);
                            }
                        }
                        else
                        {
                            if (ImGui::MenuItem("Add Event Track"))
                            {
                                AddEventTrackFromAnimation(anim);
                            }
                        }

                        ImGui::EndPopup();
                    }

                    ImGui::PopID();
                }
            }
        }
    }
    else
    {
        ImGui::Text("Not loaded Animation Track Set file");
        ImGui::Text("Please load or create a new Animation Track Set.");
    }
    ImGui::EndChild();
}

void EditorAnimationTrackTool::DrawCanvas() 
{
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(availSize.x - 400, availSize.y);
    ImGui::BeginChild("SequencerCanvas", canvasSize, true, ImGuiWindowFlags_NoScrollWithMouse);
    _sequencer->Show();
    ImGui::EndChild();
}

void EditorAnimationTrackTool::DrawDetails() 
{
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("DetailFrame", canvasSize, true);
    const auto& path = _animationEventTrack.GetFilePath();
    std::string pathStr = path.generic_string();
    ImGui::InputText("##path_input", &pathStr, ImGuiInputTextFlags_ReadOnly);
    ImVec2 size = ImGui::GetItemRectSize();
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text(pathStr.c_str());
        ImGui::EndTooltip();
    }
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FILE, ImVec2(size.y, size.y)))
    {
        NewFileWithDialog();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("New AnimationTrack Set");

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN, ImVec2(size.y, size.y)))
    {
        LoadFileWithDialog();
    }
    if (ImGui::IsItemHovered()) 
        ImGui::SetTooltip("Open AnimationTrack Set");

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FILE_SAVE, ImVec2(size.y, size.y)))
    {
        const File::Path& filePath = _animationEventTrack.GetFilePath();
        if (true == filePath.IsNull())
        {
            SaveFileWithDialog();
        }
        else
        {
            _animationEventTrack.SaveFile(filePath, true);
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Save AnimationTrack Set");

    auto curTimeline = _animationEventTrack.GetActiveEventTrack();
    if (ImGui::BeginTabBar("##AnimationTrackTabs"))
    {
        if (ImGui::BeginTabItem(TAB_LABEL[0]))
        {
            if (ShowEventTrackList(curTimeline))
            {
                FocusDetailTabItem(1);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TAB_LABEL[1]))
        {
            UINT selected = _sequencer->GetSelectedContextID();
            ShowEventTrackEditTab(curTimeline, selected);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TAB_LABEL[2]))
        {
            if (_sequencer)
            {
                _sequencer->ShowDebugData();
            }
            ImGui::EndTabItem();
        }

        if (_tabbar = ImGui::GetCurrentTabBar())
        {
            auto& tabItems = _tabbar->Tabs;
            _tabItem[0]    = &tabItems[0];
            _tabItem[1]    = &tabItems[1];
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

bool EditorAnimationTrackTool::NewFileWithDialog()
{
    File::Path out;
    LPCWSTR    title        = L"New AnimationTrack File";
    LPCWSTR    initialDir   = L"";
    LPCWSTR    defaultName  = AnimationEventTrack::DEFAULT_NAME.c_str();

    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), title, initialDir, defaultName, {}, out))
    {
        _animationEventTrack.NewFile(out);
        return true;
    }
    else
    {
        return false;
    }
}

bool EditorAnimationTrackTool::LoadFileWithDialog()
{
    std::vector<File::Path> out;
    HWND    owner   = UmApplication.GetHwnd();
    LPCWSTR title   = L"Open AnimationTrack File";
    std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"Animation Track File\0", L"*.UmAnimEvent*\0"},
                                                        {L"All File\0", L"*.*\0"}};
    
    if (File::ShowOpenFileDialog(owner, title, L"", filters, false, out))
    {
        _animationEventTrack.LoadFile(out.front());
        return true;
    }
    else
    {
        return false;
    }
}

bool EditorAnimationTrackTool::SaveFileWithDialog() 
{
    File::Path returnPath;
    LPCWSTR    title       = L"Save AnimationTrack File";
    LPCWSTR    initialDir  = L"";
    LPCWSTR    defaultName = AnimationEventTrack::DEFAULT_NAME.c_str();
    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), title, initialDir, defaultName, {}, returnPath))
    {
        _animationEventTrack.SaveFile(returnPath);
        return true;
    }
    else
    {
        return false;
    }
}

void EditorAnimationTrackTool::SetEventTrackFromAnimation(std::string_view animKey)
{
    std::string strKey(animKey);
    _eventQueue.push([this, strKey]() {
        if (nullptr == _sequencer) return;
        if (false == IsLoadAsset()) return;
        _animationEventTrack.SetActiveEventTrack(strKey);
        auto track = _animationEventTrack.GetActiveEventTrack();
        _sequencer->SetEventTrack(track);
    });
}

void EditorAnimationTrackTool::AddEventTrackFromAnimation(std::string_view animKey)
{
    std::string strKey(animKey);
    _eventQueue.push([this, strKey]() {
        if (nullptr == _sequencer) return;
        if (false == IsLoadAsset()) return;
        _animationEventTrack.AddEventTrack(strKey, true);
        auto track = _animationEventTrack.GetActiveEventTrack();
        if (nullptr != track)
        {
            auto animator = _modelDetails->GetAnimator();
            if (nullptr != animator)
            {
                _modelDetails->ChangeAnimation(strKey.data());
                track->SetMinFrame(0.0f);
                track->SetMaxFrame(animator->GetCurrentAnimationLastTime());
            }
        }
        _sequencer->SetEventTrack(track);
    });
}

void EditorAnimationTrackTool::AddEvent(std::string_view label, std::string_view animKey, std::string_view typeNameID, float time) 
{
    std::string strKey(animKey);
    std::string strName(label);
    _eventQueue.push([this, strName, strKey, typeNameID, time]() {
        if (false == IsLoadAsset()) return;
        auto timeline = _animationEventTrack.GetEventTrack(strKey);
        if (nullptr == _sequencer || nullptr == timeline) return;
        float notifyTime = time;
        if (time < 0.0f || time > timeline->GetMaxFrame())
        {
            notifyTime = timeline->GetCurrentFrame();
        }
        timeline->AddEventEx(strName, typeNameID, notifyTime);
    });
}

void EditorAnimationTrackTool::RemoveEventTrackFromAnimation(std::string_view animKey)
{
    _eventQueue.push([this, animKey]()  {
        if (false == IsLoadAsset()) return;
        if (nullptr == _sequencer) return;
        _animationEventTrack.RemoveEventTrack(animKey);
        _sequencer->SetEventTrack(std::weak_ptr<Timeline::EventTrack>());
    });
}

bool EditorAnimationTrackTool::ShowEventTrackList(std::shared_ptr<Timeline::EventTrack> track)
{
    bool doubleClicked = false;
    if (nullptr == track)
    {
        ShowAvailableEventTracks();
        return doubleClicked;
    }
    auto contextQueue = track->GetEventContextQueue();

    for (const auto& notify : contextQueue)
    {
        UINT        ID         = notify->ID;
        float       time       = notify->GetTime();
        const auto& label      = notify->GetLabel();
        bool        isSelected = (ID == _sequencer->GetSelectedContextID());

        std::string text = std::format("{:.3f} : {}", time, label);
        if (ImGui::Selectable(text.c_str(), isSelected))
        {
            _sequencer->SetSelectedContextID(ID);
            _sequencer->SetViewPositionFromID(ID, Timeline::SequencerEditor::ALIGN_CENTER);
            track->SetCurrentFrame(time);
        }
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            doubleClicked = true;
        }
    }
    if (ImGui::Selectable("+ Add Track##Details"))
    {
        ImGui::OpenPopup("##AddTrackPopup");
    }
    static char notifyBuf[64] = "\0";
    ImVec2      popupSize     = ImVec2(200.0f, 200.0f);
    ImGui::SetNextWindowSizeConstraints(popupSize, popupSize);

    if (ImGui::BeginPopup("##AddTrackPopup"))
    {
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availSize.x);
        ImGui::InputTextWithHint("##TrackLabel", "Label Name...", notifyBuf, sizeof(notifyBuf));
        ImGui::BeginChild("##TrackList", ImVec2(availSize.x, availSize.y - 30.0f), true);
        const auto& animation = GetCurrentEventTrackmName();
        const auto& table     = Timeline::EventTrack::GetInstanceConstructors();
        for (const auto& [key, func] : table)
        {
            ImGui::Text(EditorIcon::ICON_BELL_ON);
            ImGui::SameLine();
            if (ImGui::Selectable(key.c_str() + 6))
            {
                std::string label(notifyBuf);
                if (label.empty())
                    label = key.c_str() + 6;
                AddEvent(label, animation, key);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(key.c_str() + 6);
            }
        }
        ImGui::EndChild();
        ImGui::EndPopup();
    }
    else
    {
        notifyBuf[0] = '\0'; // Clear the buffer when popup is closed
    }
    return doubleClicked;
}

void EditorAnimationTrackTool::ShowEventTrackEditTab(std::shared_ptr<Timeline::EventTrack> track, UINT contextID)
{
    if (nullptr == track)
    {
        ShowAvailableEventTracks();
        return;
    }
    auto context = track->GetContextFromID(contextID);
    if (nullptr != context)
    {
        ImGui::PushID(track.get());

        context->ImGuiDrawPropertys();

        ImGui::PopID();
    }
       
    else
    {
        if (0 == contextID)
            ImGui::Text("No context selected.");
        else
            ImGui::Text("Track with ID %d not found.", contextID);
    }
}

void EditorAnimationTrackTool::ShowAvailableEventTracks()
{
    auto curTimeline = _animationEventTrack.GetActiveEventTrackName();
    if (false == curTimeline.empty())
    {
        ImGui::Text("No event track available.");
        if (ImGui::Button("+ Add Event Track"))
        {
            AddEventTrackFromAnimation(curTimeline);
        }
    }
    else
    {
        ImGui::Text("No current animation selected.");
    }
}

void EditorAnimationTrackTool::FocusDetailTabItem(int index) 
{
    if (nullptr != _tabbar && index >= 0 && index < 2)
    {
        if (_tabItem[index])
        {
            _tabbar->NextSelectedTabId = _tabItem[index]->ID;
        }
    }
}

void EditorAnimationTrackTool::LowerFramePopup(Timeline::EventTrack& track)
{
    if (ImGui::MenuItem("Paste Event"))
    {
        float time = _sequencer->GetFrameFromIndicate();
        track.PasteContext(_copyBuffer, time);
    }
    ImGui::Separator();
    if (_sequencer)
    {
        if (ImGui::MenuItem("+ Add Empty Event"))
        {
            float currentFrame = _sequencer->GetFrameFromIndicate();
            track.AddEvent<Timeline::EventContext>("New Event", currentFrame);
        }
        if (ImGui::MenuItem("+ Add Audio Event"))
        {
            float currentFrame = _sequencer->GetFrameFromIndicate();
            track.AddEvent<Timeline::AudioEventContext>("New Audio Event", currentFrame);
        }
        if (ImGui::MenuItem("+ Add Particle Event"))
        {
            float currentFrame = _sequencer->GetFrameFromIndicate();
            track.AddEvent<Timeline::ParticleEventContext>("New Particle Event", currentFrame);
        }
        if (ImGui::MenuItem("+ Add Input Event"))
        {
            float currentFrame = _sequencer->GetFrameFromIndicate();
            track.AddEvent<Timeline::InputEventContext>("New Input Event", currentFrame);
        }
    }
}

void EditorAnimationTrackTool::ContextPopup(Timeline::EventTrack& track, Timeline::EventContext& context)
{
    if (ImGui::MenuItem("Copy Event"))
    {
        _copyBuffer = track.CopyContext(&context);
    }
}

const std::string& EditorAnimationTrackTool::GetCurrentDetailAnimName()
{
    return _modelDetails->GetCurrentAnimationName();
}

const std::string& EditorAnimationTrackTool::GetCurrentEventTrackmName()
{
    return _animationEventTrack.GetActiveEventTrackName();
}

bool EditorAnimationTrackTool::IsLoadAsset() const
{
    return !_animationEventTrack.GetFilePath().IsNull();
}
