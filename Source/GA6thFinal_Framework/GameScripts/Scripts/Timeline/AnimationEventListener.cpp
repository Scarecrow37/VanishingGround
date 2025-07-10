#include "pchScripts.h"
#include "AnimationEventListener.h"
#include <Scripts/Mesh/SkeletalMeshRenderer.h>

AnimationEventListener::AnimationEventListener()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data    = (DragDropAsset::Data*)payLoad->Data;
                auto                 context = data->pContext->lock();
                if (nullptr != context)
                {
                    const auto& path      = context->GetPath();
                    const auto  extension = path.extension();
                    if (extension == AnimationNotifySet::EXTENSION)
                    {
                        SetAnimationNotifyFromPath(path);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}
AnimationEventListener::~AnimationEventListener()
{

}

void AnimationEventListener::Start()
{
    _skeletalMeshRenderer = GetComponent<SkeletalMeshRenderer>();
}

void AnimationEventListener::Update() 
{
    if (true == IsLoadedSkeletalMeshRenderer() &&
        true == IsLoadedAnimationNotifySet())
    {
        const auto& src = _skeletalMeshRenderer->GetCurrentAnimationName();
        const auto& dst = _animationNotifySet.GetActiveTimelineName();
        float srcTime   = _skeletalMeshRenderer->GetCurrentAnimationTime();
        bool isDirty    = (src != dst);
        if (true == isDirty)
        {
            _animationNotifySet.SetActiveTimeline(src);
        }
        auto timeline = _animationNotifySet.GetActiveTimeline();
        if (nullptr != timeline)
        {
            timeline->SetCurrentFrame(srcTime, isDirty);
            timeline->Update();
        }
    }
}

void AnimationEventListener::SerializedReflectEvent() 
{
    ReflectFields->Guid = _guidRef.string();
}

void AnimationEventListener::DeserializedReflectEvent() 
{
    SetAnimationNotifyFromGuid(ReflectFields->Guid);
}

void AnimationEventListener::ImGuiDrawPropertysEvent() 
{
    DrawGuiNotifiesTable();
}

bool AnimationEventListener::IsLoadedAnimationNotifySet() const
{
    return _animationNotifySet.IsLoadedFile();
}

bool AnimationEventListener::IsLoadedSkeletalMeshRenderer() const
{
    return nullptr != _skeletalMeshRenderer;
}

void AnimationEventListener::SetAnimationNotifyFromGuid(const File::Guid& guid)
{
    _guidRef  = guid;
    _filePath = guid;
    ReflectFields->Guid = _guidRef.string();

    _animationNotifySet.LoadFile(_filePath);
}

void AnimationEventListener::SetAnimationNotifyFromPath(const File::Path& path) 
{
    _guidRef  = path;
    _filePath = path;
    ReflectFields->Guid = _guidRef.string();

    _animationNotifySet.LoadFile(_filePath);
}

void AnimationEventListener::DrawGuiNotifiesTable()
{
    if (false == IsLoadedAnimationNotifySet())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Text("No animation notify set loaded.");
        ImGui::PopStyleColor();
        return;
    }
    ImGui::Separator();
    // 액티브가 아니라 선택할 수 있게.
    ImGui::Text("Timeline: ");
    const auto& curAnimName = _animationNotifySet.GetActiveTimelineName();
    const auto& animTable   = _animationNotifySet.GetTimelineTable();
    if (ImGui::BeginCombo("##timelines", curAnimName.c_str()))
    {
        for (auto& [anim, timeline] : animTable)
        {
            bool isSelected = (anim == curAnimName);
            if (ImGui::Selectable(anim.c_str(), isSelected))
            {
                _animationNotifySet.SetActiveTimeline(anim);
            }
        }
        ImGui::EndCombo();
    }

    std::shared_ptr<TimelineSystem> timeline;
    timeline = _animationNotifySet.GetActiveTimeline();

    if (nullptr == timeline)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        ImGui::Text("Timeline not found.");
        ImGui::PopStyleColor();
    }
    else
    {
        auto notifyList = timeline->GetTimelineNotifyList();
        if (ImGui::BeginTable("NotifieTable##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch, 0.15f);
            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.85f);
            ImGui::TableHeadersRow();

            for (const auto& notify : notifyList)
            {
                if (notify != nullptr)
                {
                    UINT             ID    = notify->ID;
                    float            time  = notify->Time;
                    std::string_view label = notify->Label;

                    ImGui::PushID(notify);
                    ImGui::TableNextRow();
                    {
                        ImGui::TableSetColumnIndex(0);
                        std::string timeStr = std::format("{:.3f}", time);
                        if (ImGui::Selectable(timeStr.c_str()))
                        {
                        }
                    }
                    {
                        ImGui::TableSetColumnIndex(1);
                        if (ImGui::Selectable(label.data()))
                        {
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
        }
    }
}