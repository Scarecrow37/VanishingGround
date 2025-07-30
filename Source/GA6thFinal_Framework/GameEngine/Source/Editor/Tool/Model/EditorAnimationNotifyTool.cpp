#include "pch.h"
#include "EditorAnimationNotifyTool.h"

EditorAnimationNotifyTool::EditorAnimationNotifyTool() 
{
    SetLabel("Sequencer##model");
    SetDockLayout(ImGuiDir_Down);
    SetImGuiWindowFlag(ImGuiWindowFlags_MenuBar);

    _sequencer = new Timeline::SequencerEditor();
}

EditorAnimationNotifyTool::~EditorAnimationNotifyTool() 
{
    if (nullptr != _sequencer)
    {
        delete _sequencer;
        _sequencer = nullptr;
    }
}

void EditorAnimationNotifyTool::OnTickGui() 
{
    if (nullptr == _modelDetails)
    {
        GetModelDetailsToolInDock();
    }
    while (false == _eventQueue.empty())
    {
        _eventQueue.front()();
        _eventQueue.pop();
    }
}

void EditorAnimationNotifyTool::OnStartGui() 
{
    GetModelDetailsToolInDock();
}

void EditorAnimationNotifyTool::OnEndGui() {}

void EditorAnimationNotifyTool::OnPreFrameBegin() {}

void EditorAnimationNotifyTool::OnPostFrameBegin() {}

void EditorAnimationNotifyTool::OnFrameRender() 
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

void EditorAnimationNotifyTool::OnFrameEnd() {}

void EditorAnimationNotifyTool::OnFrameFocusEnter() {}

void EditorAnimationNotifyTool::OnFrameFocusStay() {}

void EditorAnimationNotifyTool::OnFrameFocusExit() {}

void EditorAnimationNotifyTool::OnFramePopupOpened() {}

void EditorAnimationNotifyTool::SerializedReflectEvent() {}

void EditorAnimationNotifyTool::DeserializedReflectEvent() {}

void EditorAnimationNotifyTool::GetModelDetailsToolInDock()
{
    const auto* dock = GetOwnerDockWindow();
    _modelDetails = dock->GetGui<EditorModelDetails>();
}

void EditorAnimationNotifyTool::UpdateTimeline() 
{
    auto curDetailAnim = _modelDetails->GetCurrentAnimationName();
    auto curNotifyAnim = _animationNotifySet.GetActiveTimelineName();

    bool isSameAnim = !curDetailAnim.empty() && !curNotifyAnim.empty() && (curDetailAnim == curNotifyAnim);

    auto model    = _modelDetails->GetModel();
    auto animator = _modelDetails->GetAnimator();
    if (true == isSameAnim)
    {
        if (model && animator)
        {
            auto timeline = _animationNotifySet.GetActiveTimeline();
            if (nullptr != timeline)
            {
                timeline->SetMinFrame(0.0f);
                timeline->SetMaxFrame(animator->GetCurrentAnimationLastTime());
                timeline->SetCurrentFrame(animator->GetCurrentAnimationPlayTime());
                _sequencer->RemoveFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
                timeline->Update();
            }
        }
    }
    else
    {
        _sequencer->AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_DRAG_CURRENT_LINE);
    }
}

void EditorAnimationNotifyTool::DrawMenuBar()
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
                const File::Path& filePath = _animationNotifySet.GetFilePath();
                if (true == filePath.IsNull())
                {
                    SaveFileWithDialog();
                }
                else
                {
                    _animationNotifySet.SaveFile(filePath);
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

void EditorAnimationNotifyTool::DrawTimelines() 
{
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(200.0f, availSize.y);
    ImGui::BeginChild("SequencerTimelines", canvasSize, true);

    if (true == IsLoadNotifySet())
    {
        const auto& table = _animationNotifySet.GetTimelineTable();
        ImGui::Text("Timeline Count: %d", table.size());
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
                    auto& curNotifyAnim = GetCurrentNotifyAnimName();
                    bool  isPlaying     = !curDetailAnim.empty() && curDetailAnim == anim;
                    bool  isSeleted     = !curNotifyAnim.empty() && curNotifyAnim == anim;

                    ImGui::PushID(anim);
                    // Check timeline to anim in set
                    auto timeline = _animationNotifySet.GetTimeline(anim);
                    std::function<void()> selectablefunc = nullptr;
                    std::string           selectableLabel;
                    if (nullptr != timeline)
                    {
                        selectableLabel = std::format("{}  {}", EditorIcon::ICON_BELL_ON, anim);
                        if (true == isPlaying)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
                        }
                        else
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
                        }
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
                        selectableLabel = std::format("{}  {}", EditorIcon::ICON_BELL_OFF, anim);
                    }

                    if (ImGui::Selectable(selectableLabel.c_str(), isSeleted))
                    {
                        SetTimelineFromAnimation(anim);
                        if (selectablefunc) 
                            selectablefunc();
                    }

                    if (true == isPlaying)
                    {
                        ImVec2 old = ImGui::GetCursorPos();
                        ImVec2 size = ImGui::GetItemRectSize();
                        ImGui::SameLine(1.5f);
                        ImGuiHelper::LoadingSpinner(size.y * 0.5f, ImColor(100, 255, 100, 255));
                        ImGui::SetCursorPos(old);
                    }
                    ImGui::PopStyleColor();

                    // Context
                    if (ImGui::BeginPopupContextItem("TimelineContextMenu"))
                    {
                        if (nullptr != timeline)
                        {
                            if (ImGui::BeginMenu("Add Notify"))
                            {
                                auto& table = Timeline::EventTrack::GetInstanceConstructors();
                                for (const auto& [key, func] : table)
                                {
                                    if (ImGui::MenuItem(key.c_str() + 6))
                                    {
                                        AddNotify(key.c_str() + 6, anim, key);
                                    }
                                }
                                ImGui::EndMenu();
                            }
                            if (ImGui::MenuItem("Remove Timeline"))
                            {
                                RemoveTimelineFromAnimation(anim);
                            }
                        }
                        else
                        {
                            if (ImGui::MenuItem("Add Timeline"))
                            {
                                AddTimelineFromAnimation(anim);
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
        ImGui::Text("Not loaded Animation Notify Set file");
        ImGui::Text("Please load or create a new Animation Notify Set.");
    }
    ImGui::EndChild();
}

void EditorAnimationNotifyTool::DrawCanvas() 
{
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(availSize.x - 400, availSize.y);
    ImGui::BeginChild("SequencerCanvas", canvasSize, true, ImGuiWindowFlags_NoScrollWithMouse);
    _sequencer->Show();
    ImGui::EndChild();
}

void EditorAnimationNotifyTool::DrawDetails() 
{
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("DetailFrame", canvasSize, true);
    static char pathBuffer[128] = "";
    const auto& path = _animationNotifySet.GetFilePath();
    std::string pathStr = path.generic_string();
    strcpy_s(pathBuffer, pathStr.c_str());
    ImGui::InputText("##path_input", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_ReadOnly);
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
        ImGui::SetTooltip("New AnimationNotify Set");

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN, ImVec2(size.y, size.y)))
    {
        LoadFileWithDialog();
    }
    if (ImGui::IsItemHovered()) 
        ImGui::SetTooltip("Open AnimationNotify Set");

    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FILE_SAVE, ImVec2(size.y, size.y)))
    {
        const File::Path& filePath = _animationNotifySet.GetFilePath();
        if (true == filePath.IsNull())
        {
            SaveFileWithDialog();
        }
        else
        {
            _animationNotifySet.SaveFile(filePath);
        }
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Save AnimationNotify Set");

    auto curTimeline = _animationNotifySet.GetActiveTimeline();
    if (ImGui::BeginTabBar("##AnimationNotifyTabs"))
    {
        if (ImGui::BeginTabItem(_tabLabel[0].c_str()))
        {
            ShowNotifyList(curTimeline);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(_tabLabel[1].c_str()))
        {
            UINT selected = _sequencer->GetSelectedContextID();
            ShowNotifyEditTab(curTimeline, selected);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

bool EditorAnimationNotifyTool::NewFileWithDialog()
{
    File::Path out;
    LPCWSTR    title        = L"New AnimationNotify File";
    LPCWSTR    initialDir   = L"";
    LPCWSTR    defaultName  = AnimationNotifySet::DEFAULT_NAME.c_str();

    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), title, initialDir, defaultName, {}, out))
    {
        _animationNotifySet.NewFile(out);
        return true;
    }
    else
    {
        return false;
    }
}

bool EditorAnimationNotifyTool::LoadFileWithDialog()
{
    std::vector<File::Path> out;
    HWND    owner   = UmApplication.GetHwnd();
    LPCWSTR title   = L"Open AnimationNotify File";
    std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"AnimationNotify File\0", L"*.UmAnimNotifySet*\0"},
                                                        {L"All File\0", L"*.*\0"}};
    
    if (File::ShowOpenFileDialog(owner, title, L"", filters, false, out))
    {
        _animationNotifySet.LoadFile(out.front());
        return true;
    }
    else
    {
        return false;
    }
}

bool EditorAnimationNotifyTool::SaveFileWithDialog() 
{
    File::Path returnPath;
    LPCWSTR    title       = L"Save AnimationNotify File";
    LPCWSTR    initialDir  = L"";
    LPCWSTR    defaultName = AnimationNotifySet::DEFAULT_NAME.c_str();
    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), title, initialDir, defaultName, {}, returnPath))
    {
        _animationNotifySet.SaveFile(returnPath);
        return true;
    }
    else
    {
        return false;
    }
}

void EditorAnimationNotifyTool::SetTimelineFromAnimation(std::string_view animKey) 
{
    std::string strKey(animKey);
    _eventQueue.push([this, strKey]() {
        if (nullptr == _sequencer) return;
        if (false == IsLoadNotifySet()) return;
        _animationNotifySet.SetActiveTimeline(strKey);
        auto track = _animationNotifySet.GetActiveTimeline();
        _sequencer->SetEventTrack(track);
    });
}

void EditorAnimationNotifyTool::AddTimelineFromAnimation(std::string_view animKey)
{
    std::string strKey(animKey);
    _eventQueue.push([this, strKey]() {
        if (nullptr == _sequencer) return;
        if (false == IsLoadNotifySet()) return;
        _animationNotifySet.AddTimeline(strKey, true);
        auto track = _animationNotifySet.GetActiveTimeline();
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

void EditorAnimationNotifyTool::AddNotify(std::string_view notifyName, std::string_view animKey, std::string_view typeNameID, float time) 
{
    std::string strKey(animKey);
    std::string strName(notifyName);
    _eventQueue.push([this, strName, strKey, typeNameID, time]() {
        if (false == IsLoadNotifySet()) return;
        auto timeline = _animationNotifySet.GetTimeline(strKey);
        if (nullptr == _sequencer || nullptr == timeline) return;
        float notifyTime = time;
        if (time < 0.0f || time > timeline->GetMaxFrame())
        {
            notifyTime = timeline->GetCurrentFrame();
        }
        timeline->AddEventEx(strName, typeNameID, notifyTime);
    });
}

void EditorAnimationNotifyTool::RemoveTimelineFromAnimation(std::string_view animKey)
{
    _eventQueue.push([this, animKey]()  {
        if (false == IsLoadNotifySet()) return;
        if (nullptr == _sequencer) return;
        _animationNotifySet.RemoveTimeline(animKey);
        _sequencer->SetEventTrack(std::weak_ptr<Timeline::EventTrack>());
    });
}

bool EditorAnimationNotifyTool::ShowNotifyList(std::shared_ptr<Timeline::EventTrack> track)
{
    bool itemClicked = false;
    if (nullptr == track)
    {
        ShowAvailableTimeline();
        return itemClicked;
    }
    auto contextQueue = track->GetEventContextQueue();
    if (ImGui::BeginTable("NotifieTable##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Time" ,ImGuiTableColumnFlags_WidthStretch, 0.15f);
        ImGui::TableSetupColumn("Label",ImGuiTableColumnFlags_WidthStretch, 0.85f);
        ImGui::TableHeadersRow();

        for (const auto& notify : contextQueue)
        {
            if (notify != nullptr)
            {
                UINT             ID    = notify->ID;
                float            time  = notify->Time;
                std::string_view label = notify->Label;
                bool             isSelected = (ID == _sequencer->GetSelectedContextID());

                ImGui::PushID(notify);
                ImGui::TableNextRow();
                {
                    ImGui::TableSetColumnIndex(0);
                    std::string timeStr = std::format("{:.3f}", time);
                    if (ImGui::Selectable(timeStr.c_str(), isSelected))
                    {
                        _sequencer->SetViewPositionFromID(ID, Timeline::SequencerEditor::ALIGN_CENTER);
                        track->SetCurrentFrame(time);
                        itemClicked = true;
                    }
                }
                {
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::Selectable(label.data(), isSelected))
                    {
                        _sequencer->SetSelectedContextID(ID);
                        _sequencer->SetViewPositionFromID(ID, Timeline::SequencerEditor::ALIGN_CENTER);
                        itemClicked = true;
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("ID: %d\nTime: %.2f", ID, time);
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();

        if (ImGui::Selectable("+ Add Notify##Details"))
        {
            ImGui::OpenPopup("##AddNotifyPopup");
        }
        static char notifyBuf[64] = "\0";
        ImVec2 popupSize = ImVec2(200.0f, 200.0f);
        ImGui::SetNextWindowSizeConstraints(popupSize, popupSize);
        
        if (ImGui::BeginPopup("##AddNotifyPopup"))
        {
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x);
            ImGui::InputTextWithHint("##NotifyLabel", "Label Name...", notifyBuf, sizeof(notifyBuf));
            ImGui::BeginChild("##NotifyList", ImVec2(availSize.x, availSize.y - 30.0f), true);
            const auto& animation = GetCurrentNotifyAnimName();
            auto&       table     = Timeline::EventTrack::GetInstanceConstructors();
            for (const auto& [key, func] : table)
            {
                ImGui::Text(EditorIcon::ICON_BELL_ON);
                ImGui::SameLine();
                if (ImGui::Selectable(key.c_str() + 6))
                {
                    std::string label(notifyBuf);
                    if (label.empty()) label = key.c_str() + 6;
                    AddNotify(label, animation, key);
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
    }
    return itemClicked;
}

void EditorAnimationNotifyTool::ShowNotifyEditTab(std::shared_ptr<Timeline::EventTrack> track, UINT contextID)
{
    if (nullptr == track)
    {
        ShowAvailableTimeline();
        return;
    }
    auto context = track->GetContextFromID(contextID);
    if (nullptr != context)
    {
        ImGui::PushID(track.get());

        std::string_view label = context->Label;
        float            time  = context->Time;
        UINT             id    = context->ID;
        {
            char buf[64];
            strcpy_s(buf, label.data());
            ImGui::Text("Label: ");
            ImGui::Indent();
            if (ImGui::InputText("##NotifyLabel", buf, sizeof(buf)))
            {
                context->Label = buf;
            }
            ImGui::Unindent();
        }
        ImGui::Separator();
        {
            ImGui::Text("Event: ");
            ImGui::Indent();
            std::string_view eventName = context->EventName;
            auto table = Timeline::EventTrack::GetInstanceConstructors();
            if (ImGui::BeginCombo("##EventName", eventName.data() + 6))
            {
                for (const auto& [key, func] : table)
                {
                    std::string curEvent(eventName);
                    bool isSelected = (curEvent == key);
                    const char* label = key.c_str() + 6;
                    if (ImGui::Selectable(label, isSelected))
                    {
                        context->SetEvent(key);
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Unindent();
        }

        ImGui::PopID();
    }
       
    else
    {
        if (0 == contextID)
            ImGui::Text("No context selected.");
        else
            ImGui::Text("Notify with ID %d not found.", contextID);
    }
}

void EditorAnimationNotifyTool::ShowAvailableTimeline() 
{
    auto curTimeline = _animationNotifySet.GetActiveTimelineName();
    if (false == curTimeline.empty())
    {
        ImGui::Text("No timeline available.");
        if (ImGui::Button("+ Add Timeline"))
        {
            AddTimelineFromAnimation(curTimeline);
        }
    }
    else
    {
        ImGui::Text("No current animation selected.");
    }
}

const std::string& EditorAnimationNotifyTool::GetCurrentDetailAnimName()
{
    return _modelDetails->GetCurrentAnimationName();
}

const std::string& EditorAnimationNotifyTool::GetCurrentNotifyAnimName()
{
   return _animationNotifySet.GetActiveTimelineName();
}

bool EditorAnimationNotifyTool::IsLoadNotifySet() const
{
    return !_animationNotifySet.GetFilePath().IsNull();
}
