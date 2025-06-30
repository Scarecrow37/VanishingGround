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
    auto model    = _modelDetails->GetModel();
    auto animator = _modelDetails->GetAnimator();
    if (model && animator)
    {
        std::string curAnim  = _modelDetails->GetCurrentAnimationName();
        _animationNotifySet.SetActiveTimeline(curAnim);
        auto timeline = _animationNotifySet.GetActiveTimeline();
        if (nullptr != timeline)
        {
            _sequencer->SetSystem(timeline);
            timeline->SetMinFrame(0.0f);
            timeline->SetMaxFrame(animator->GetCurrentAnimationLastTime());
            timeline->SetCurrentFrame(animator->GetCurrentAnimationPlayTime());
        }
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
        ImGui::EndMenuBar();
    }
}

void EditorAnimationNotifyTool::DrawTimelines() 
{
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize = ImVec2(200.0f, availSize.y);
    ImGui::BeginChild("SequencerTimelines", canvasSize, true);
    const auto& table = _animationNotifySet.GetTimelineTable();
    for (const auto& [animKey, timeline] : table)
    {
        if (nullptr != timeline)
        {
            ImGui::PushID(timeline.get());
            if (ImGui::Selectable(animKey.c_str(), _animationNotifySet.GetActiveTimeline() == timeline))
            {
                _animationNotifySet.SetActiveTimeline(animKey);
                _sequencer->SetSystem(timeline);
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

    auto model    = _modelDetails->GetModel();
    auto animator = _modelDetails->GetAnimator();
    if (model && animator)
    {
        std::string curAnim   = _modelDetails->GetCurrentAnimationName();
        auto        curSystem = _animationNotifySet.GetTimeline(curAnim);
        if (nullptr == curSystem)
        {
            ImGui::Text("No active timeline selected.");
            // AddTimeline
            if (ImGui::Button("Add Timeline"))
            {
                _animationNotifySet.AddTimeline(curAnim);
                _animationNotifySet.SetActiveTimeline(curAnim);
                auto timeline = _animationNotifySet.GetActiveTimeline();
                _sequencer->SetSystem(timeline);
            }
        }
        else
        {
            // ImGui::Text("Active Timeline: %s", curSystem->GetAnimKey().c_str());
            ImGui::Text("Current Frame: %.2f", curSystem->GetCurrentFrame());
            ImGui::Text("Min Frame: %.2f", curSystem->GetMinFrame());
            ImGui::Text("Max Frame: %.2f", curSystem->GetMaxFrame());
        }

        
    }
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
