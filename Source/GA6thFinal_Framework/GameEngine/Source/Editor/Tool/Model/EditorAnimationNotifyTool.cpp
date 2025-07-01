#include "pch.h"
#include "EditorAnimationNotifyTool.h"
#include "Engine/GraphicsCore/Model.h"
#include "Engine/GraphicsCore/Animation.h"
#include "Engine/GraphicsCore/Animator.h"

#include "../Sequencer/EditorSequencerTool.cpp"

EditorAnimationNotifyTool::EditorAnimationNotifyTool() 
{
    SetLabel("Sequencer##model");
    SetDockLayout(ImGuiDir_Right);
    SetImGuiWindowFlag(ImGuiWindowFlags_MenuBar);

    _sequencer = new EditorSequencer();
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
                _sequencer->RemoveFlags(EditorSequencer::FLAGS_USE_DRAG_FRAME_LINE);
            }
        }
    }
    else
    {
        _sequencer->AddFlags(EditorSequencer::FLAGS_USE_DRAG_FRAME_LINE);
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
                File::Path returnPath;
                if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), L"New AnimationNotify File", L"",
                                             AnimationNotifySet::DEFAULT_NAME.c_str(), {}, returnPath))
                {
                    _animationNotifySet.NewFile(returnPath);
                }
            }
            if (ImGui::MenuItem("Open File"))
            {
                HWND    owner = UmApplication.GetHwnd();
                LPCWSTR title = L"Open AnimationNotify File";
                std::vector<std::pair<LPCWSTR,LPCWSTR>> filters = {{L"AnimationNotify File\0", L"*.UmAnimNotifySet*\0"}, {L"All File\0", L"*.*\0"}};
                std::vector<File::Path> out;
                if (File::ShowOpenFileDialog(owner, title, L"", filters, false, out))
                {
                    _animationNotifySet.LoadFile(out.front());
                }
            }
            if (ImGui::MenuItem("Save File"))
            {
                const File::Path& filePath = _animationNotifySet.GetFilePath();
                if (true == filePath.IsNull())
                {
                    File::Path returnPath;
                    if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), L"Save AnimationNotify File", L"",
                                                 AnimationNotifySet::DEFAULT_NAME.c_str(), {}, returnPath))
                    {
                        _animationNotifySet.SaveFile(returnPath);
                    }
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
            bool useSnap = _sequencer->HasFlags(EditorSequencer::FLAGS_USE_SNAP);
            ImVec4 trueColor  = ImVec4(0.1f, 0.2f, 0.21f, 0.8f);
            ImVec4 falseColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
            if (ImGuiHelper::ToggleButton("Snap", &useSnap, trueColor, falseColor))
            {
                _sequencer->ToggleFlags(EditorSequencer::FLAGS_USE_SNAP);
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
    const auto& table = _animationNotifySet.GetTimelineTable();
    ImGui::Text("Timeline Count: %d", table.size());
    ImGui::Separator();
    for (const auto& [animKey, timeline] : table)
    {
        if (nullptr != timeline)
        {
            ImGui::PushID(timeline.get());
            // Widget
            if (ImGui::Selectable(animKey.c_str(), _animationNotifySet.GetActiveTimeline() == timeline))
            {
                _animationNotifySet.SetActiveTimeline(animKey);
                _sequencer->SetSystem(timeline);
            }
            // Context
            if (ImGui::BeginPopupContextItem("TimelineContextMenu"))
            {
                if (ImGui::BeginMenu("Add Notify"))
                {
                    auto& table = TimelineSystem::GetInstanceConstructors();
                    for (const auto& [key, func] : table)
                    {
                        if (ImGui::MenuItem(key.c_str() + 6))
                        {
                            AddNotify(animKey, key);
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Remove Timeline"))
                {
                    RemoveTimeline(animKey);
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
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
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text(pathStr.c_str());
        ImGui::EndTooltip();
    }
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN))
    {
        HWND    owner = UmApplication.GetHwnd();
        LPCWSTR title = L"새 애니메이션 노티파이 만들기";
        LPCWSTR defaultPath = L"";
        std::vector<std::pair<LPCWSTR,LPCWSTR>> filter = {{L"애니메이션 노티파이 파일\0", L"*.UmAnimNotifySet*\0"}, {L"모든 파일\0", L"*.*\0"}};
        std::vector<File::Path> out;
        if (File::ShowOpenFileDialog(owner, title, defaultPath, filter, false, out))
        {
            _animationNotifySet.LoadFile(out.front());
        }
    }
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
            UINT selected = _sequencer->GetSelectedNotifyID();
            ShowNotifyEditTab(curTimeline, selected);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    // auto model    = _modelDetails->GetModel();
    // auto animator = _modelDetails->GetAnimator();
    // if (model && animator)
    //{
    //     std::string curAnim   = _modelDetails->GetCurrentAnimationName();
    //     auto        curSystem = _animationNotifySet.GetTimeline(curAnim);
    //     if (nullptr == curSystem)
    //     {
    //         ImGui::Text("No active timeline selected.");
    //         if (ImGui::Button("Add Timeline"))
    //         {
    //             AddTimelineFromAnimation(curAnim);
    //         }
    //     }
    //     else
    //     {
    //         ImGui::Text("Current Frame: %.2f", curSystem->GetCurrentFrame());
    //         ImGui::Text("Min Frame: %.2f", curSystem->GetMinFrame());
    //         ImGui::Text("Max Frame: %.2f", curSystem->GetMaxFrame());
    //     }
    // }

    ImGui::EndChild();
    //for (const auto& notify : _timelineSystem->GetTimelineNotifyList())
    //{
    //    ImGui::PushID(notify);
    //
    //    char             buf[64];
    //    std::string_view label = notify->Label;
    //    float            time  = notify->Time;
    //    UINT             id    = notify->ID;
    //    strcpy_s(buf, label.data()); // copy the label to a buffer
    //    ImGui::Text("Label: ");
    //    ImGui::SameLine();
    //    if (ImGui::InputText("Label", buf, sizeof(buf)))
    //    {
    //        notify->Label = buf; // update the label in the notify
    //    }
    //    ImGui::Text("Time: %.2f", time);
    //    ImGui::Text("ID: %d", id);
    //    if (ImGui::TreeNodeEx("Event", ImGuiTreeNodeFlags_DefaultOpen))
    //    {
    //        std::string_view eventName = notify->EventName;
    //        ImGui::Text("Event Name: %s", eventName.data() + 6);
    //        notify->Event->ImGuiDrawPropertys();
    //        ImGui::TreePop();
    //    }
    //    ImGui::Separator();
    //
    //    ImGui::PopID();
    //}
}

void EditorAnimationNotifyTool::AddTimelineFromAnimation(std::string_view animKey) 
{
    _animationNotifySet.AddTimeline(animKey);
    _animationNotifySet.SetActiveTimeline(animKey);
    auto timeline = _animationNotifySet.GetActiveTimeline();
    _sequencer->SetSystem(timeline);

     _eventQueue.push([this, animKey]() {
        if (nullptr == _sequencer)
            return;
        _animationNotifySet.AddTimeline(animKey, true);
        auto timeline = _animationNotifySet.GetActiveTimeline();
        _sequencer->SetSystem(timeline);
    });
}

void EditorAnimationNotifyTool::AddNotify(std::string_view animKey, std::string_view typeNameID, float time) 
{
     _eventQueue.push([this, animKey, typeNameID, time]() {
        auto timeline = _animationNotifySet.GetTimeline(animKey);
        if (nullptr == _sequencer || nullptr == timeline)
            return;
        float notifyTime = time;
        if (time < 0.0f || time > timeline->GetMaxFrame())
        {
            notifyTime = timeline->GetCurrentFrame();
        }
        timeline->AddNotify(typeNameID.data() + 6, typeNameID, notifyTime);
    });
}

void EditorAnimationNotifyTool::RemoveTimeline(std::string_view animKey) 
{
    _eventQueue.push([this, animKey]()  {
        if (nullptr == _sequencer) return;
        _animationNotifySet.RemoveTimeline(animKey);
        _sequencer->SetSystem(nullptr);
    });
}

bool EditorAnimationNotifyTool::ShowNotifyList(std::shared_ptr<TimelineSystem> system)
{
    bool itemClicked = false;
    if (nullptr == system)
    {
        ImGui::Text("No timeline system available.");
        return itemClicked;
    }
    auto notifyList = system->GetTimelineNotifyList();
    for (const auto& notify : notifyList)
    {
        if (nullptr == notify)
            continue;
        UINT ID = notify->ID;
        float time = notify->Time;
        std::string_view label = notify->Label;
        ImGui::Selectable(label.data());
        bool isHovered = ImGui::IsItemHovered();
        bool isDoubleClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
        if (isHovered)
        {
            ImGui::SetTooltip("ID: %d\nTime: %.2f", ID, time);
        }
        if (isDoubleClicked)
        {
            _sequencer->SetSelectedNotifyID(ID);
            _sequencer->SetViewPositionFromID(ID);
            itemClicked = true;
        }
    }
    return itemClicked;
}

void EditorAnimationNotifyTool::ShowNotifyEditTab(std::shared_ptr<TimelineSystem> system, UINT notifyID) 
{
    if (nullptr == system)
    {
        ImGui::Text("No timeline system available.");
        return;
    }
    auto notify = system->GetNotifyFromID(notifyID);
    if (nullptr != notify)
    {
        char             buf[64];
        std::string_view label = notify->Label;
        float            time  = notify->Time;
        UINT             id    = notify->ID;
        strcpy_s(buf, label.data());
        ImGui::Text("Label: ");
        ImGui::SameLine();
        if (ImGui::InputText("##NotifyLabel", buf, sizeof(buf)))
        {
            notify->Label = buf;
        }
        ImGui::Separator();
    }
    else
    {
        if (0 == notifyID)
            ImGui::Text("No notify selected.");
        else
            ImGui::Text("Notify with ID %d not found.", notifyID);
    }
}
