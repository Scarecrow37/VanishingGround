#include "pchScripts.h"
#include "AnimationEventListener.h"
#include "Scripts/Mesh/SkeletalMeshRenderer.h"

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
    if (true == IsLoadedSkeletalMeshRenderer() ||
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

REGISTER_CLASS(TimelineSystem, TestTimeLineEvent_1)
void TestTimeLineEvent_1::OnNotified(float time)
{
    std::string message = "TestTimeLineEvent_1 Notify at " + std::to_string(time) + "s";
    UmLogger.Log(2, message);
}
REGISTER_CLASS(TimelineSystem, TestTimeLineEvent_2)
void TestTimeLineEvent_2::OnNotified(float time)
{
    std::string message = "TestTimeLineEvent_2 Notify at " + std::to_string(time) + "s";
    UmLogger.Log(2, message);
}
