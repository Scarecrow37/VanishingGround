#include "pchScripts.h"
#include "AnimationComponent.h"
#include <Mesh/SkeletalMeshRenderer.h>

void AnimationComponent::Reset() 
{
    _skeletalMeshRenderer = GetComponent<SkeletalMeshRenderer>();
}

void AnimationComponent::Awake()
{
    ChangeMainAnimation(ReflectFields->MainAnimationKey, false);
    ChangeMainAnimationFlags(ReflectFields->MainAnimationFlags);
}

void AnimationComponent::Update() 
{
    if (_skeletalMeshRenderer)
    {
        for (auto& animData : _overrideAnimationStack)
        {
            UpdateAnimation(animData);
        }
        UpdateAnimation(_mainAnimationData);

        for (auto& event : _eventQueue)
        {
            event();
        }
        _eventQueue.clear();
    }
    else
    {
        _skeletalMeshRenderer = GetComponent<SkeletalMeshRenderer>();
    }
}

void AnimationComponent::OnDrawDebug()
{
    // 메인 애니메이션만
    UpdateAnimation(_mainAnimationData);
    if (nullptr == _skeletalMeshRenderer)
    {
        _skeletalMeshRenderer = GetComponent<SkeletalMeshRenderer>();
    }
}

void AnimationComponent::SerializedReflectEvent()
{
    ReflectFields->MainAnimationKey   = _mainAnimationData.AnimationName;
    ReflectFields->MainAnimationFlags = _mainAnimationData.Flags;
    ReflectFields->MainAnimationSpeed = _mainAnimationData.Speed;
}

void AnimationComponent::DeserializedReflectEvent()
{
    _mainAnimationData.AnimationName = ReflectFields->MainAnimationKey;
    _mainAnimationData.Flags         = ReflectFields->MainAnimationFlags;
    _mainAnimationData.Speed         = ReflectFields->MainAnimationSpeed;
}

void AnimationComponent::ImGuiDrawPropertysEvent()
{
    if (nullptr == _skeletalMeshRenderer)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "SkeletalMeshRenderer is NULL");
    }

    Model*    model    = GetModel();
    Animator* animator = GetAnimator();
    if (model && animator)
    {
       
        const auto&    animation      = model->GetAnimation();
        const auto&    animationNames = animation->GetAnimations();
        AnimationData& curAnimData    = GetLastAnimationDataEx();
        if (ImGui::TreeNodeEx("Current Animation##details"))
        {
            const auto& animationNames = animation->GetAnimations();
            const char* comboLabel     = curAnimData.AnimationName.empty() ? "-" : curAnimData.AnimationName.c_str();
            if (ImGui::BeginCombo("##Animation", comboLabel))
            {
                for (int i = 0; i < animationNames.size(); ++i)
                {
                    bool isSelected = (curAnimData.AnimationName == animationNames[i]);
                    if (ImGui::Selectable(animationNames[i], isSelected))
                    {
                        curAnimData.AnimationName = animationNames[i];
                        ChangeCurrentAnimation(animationNames[i]);
                    }
                }
                ImGui::EndCombo();
            }
            if (true == curAnimData.AnimationName.empty())
            {
                ImGui::BeginDisabled();
            }
            {
                bool usePushStyleColor = !curAnimData.HasFlag(ANIMATION_FLAG_PAUSE);
                if (true == usePushStyleColor)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                if (ImGui::Button(EditorIcon::ICON_PLAY))
                    curAnimData.ToggleFlag(ANIMATION_FLAG_PAUSE);
                if (true == usePushStyleColor)
                    ImGui::PopStyleColor();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Play");

                ImGui::SameLine();

                if (ImGui::Button(EditorIcon::ICON_PAUSE))
                    PauseCurrentAnimation();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Pause");

                ImGui::SameLine();

                if (ImGui::Button(EditorIcon::ICON_STOP))
                    StopCurrentAnimation();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Stop");
            }
            if (true == curAnimData.AnimationName.empty())
            {
                ImGui::EndDisabled();
            }

            bool loop = curAnimData.HasFlag(ANIMATION_FLAG_USE_LOOP);
            if (ImGui::Checkbox("Loop", &loop))
            {
                curAnimData.ToggleFlag(ANIMATION_FLAG_USE_LOOP);
            }

            float min = 0.0f;
            float max = animator->GetCurrentAnimationLastTime();
            float cur = animator->GetCurrentAnimationPlayTime();
            if (ImGui::SliderFloat("Current Animation Frame", &cur, min, max))
            {
                animator->SetAnimationTime(cur);
            }
            if (ImGui::DragFloat("Animation Speed", &curAnimData.Speed, 0.01f))
            {
            }

            ImGui::TreePop();
        }

        ImGui::Separator();
        ImGuiHelper::TextWithVerticalSeparator("Animation Speed Scale");
        ImGui::DragFloat("##Animation Speed Scale", &ReflectFields->AnimationSpeedScale, 0.01f, 0.0f);
    }
}

AnimationData& AnimationComponent::GetLastAnimationDataEx()
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.back();
    }
    return _mainAnimationData;
}

MeshRenderer* AnimationComponent::GetRenderer() const
{
    if (_skeletalMeshRenderer)
    {
        return _skeletalMeshRenderer->Renderer.get();
    }
    else
    {
        return nullptr;
    }
}

Model* AnimationComponent::GetModel() const
{
    MeshRenderer* renderer = GetRenderer();
    if (renderer)
    {
        auto model = renderer->GetModel();
        if (model)
        {
            return model.get();
        }
    }
    return nullptr;
}

Animator* AnimationComponent::GetAnimator() const
{
    MeshRenderer* renderer = GetRenderer();
    if (renderer)
    {
        auto animator = renderer->GetAnimator();
        if (animator)
        {
            return animator.get();
        }
    }
    return nullptr;
}

void AnimationComponent::UpdateAnimation(AnimationData& animData)
{
    if (_skeletalMeshRenderer)
    {
        auto animator   = GetAnimator();
        bool isLastData = GetLastAnimationDataEx().IsSameData(animData);
        if (animator)
        {
            float animFrameScale = animData.Speed * ReflectFields->AnimationSpeedScale;
            if (isLastData)
            {
                animator->SetPause(animData.HasFlag(ANIMATION_FLAG_PAUSE));
                animator->SetLoop(animData.HasFlag(ANIMATION_FLAG_USE_LOOP));
                animator->SetAnimationSpeed(animFrameScale);
                animData.ElapsedFrame = animator->GetCurrentAnimationPlayTime();
            }
            else
            {
                if (animData.HasFlag(ANIMATION_FLAG_ALWAYS_UPDATE))
                {
                    float maxFrame = animator->GetAnimationLastTime(animData.AnimationName.data());
                    float delta    = UmTime.DeltaTime();
                    animData.ElapsedFrame += delta * animFrameScale;
                    if (animData.ElapsedFrame >= maxFrame)
                    {
                        animData.ElapsedFrame = maxFrame;

                        UINT id = animData.ID;
                        _eventQueue.push_back([this, id]() {
                            auto beginItr = _overrideAnimationStack.begin();
                            auto endItr   = _overrideAnimationStack.end();
                            auto itr      = std::remove_if(beginItr, endItr, [id](const AnimationData& data) { return data.IsSameID(id); });
                            _overrideAnimationStack.erase(itr, endItr);
                        });
                    }
                }
            }
            if (animData.PopCondition)
            {
                bool result = animData.PopCondition(animData);
                if (result)
                {
                    PopOverrideAnimation();
                }
            }
        }
    }
}

void AnimationComponent::SetAnimationEx(AnimationData& animData) 
{
    Animator* animator = GetAnimator();
    if (animator)
    {
        bool isLastData = GetLastAnimationDataEx().IsSameData(animData);
        if (true == isLastData && false == _isBuildingOverrideAnimation)
        {
            animator->ChangeAnimation(animData.AnimationName.c_str(), animData.IsBlending);
            if (animData.HasFlag(ANIMATION_FLAG_RESET_FRAME))
            {
                animData.ElapsedFrame = 0.0f;
            }
            animator->SetAnimationTime(animData.ElapsedFrame);
        }
    }
}

void AnimationComponent::ChangeAnimationEx(AnimationData& animData, std::string_view animKey, bool blend)
{
    Animator* animator = GetAnimator();
    if (animator)
    {
        animData.IsBlending    = blend;
        animData.AnimationName = animKey;
        animData.MaxFrame      = animator->GetAnimationLastTime(animData.AnimationName.data());
        bool isLastData        = GetLastAnimationDataEx().IsSameData(animData);
        if (true == isLastData && false == _isBuildingOverrideAnimation)
        {
            SetAnimationEx(animData);
        }
    }
}

void AnimationComponent::ChangeAnimationFrameEx(AnimationData& animData, float frame)
{
    Animator* animator = GetAnimator();
    if (animator)
    {
        animData.ElapsedFrame = std::clamp(frame, 0.0f, animData.MaxFrame);
        bool isLastData       = GetLastAnimationDataEx().IsSameData(animData);
        if (true == isLastData && false == _isBuildingOverrideAnimation)
        {
            animator->SetAnimationTime(animData.ElapsedFrame);
        }
    }
}

void AnimationComponent::ChangeAnimationFlagsEx(AnimationData& animData, int flags)
{
    Animator* animator = GetAnimator();
    if (animator)
    {
        animData.Flags = flags;
    }
}

void AnimationComponent::ClearOverrideAnimations() 
{
    if (false == _overrideAnimationStack.empty())
    {
        _overrideAnimationStack.clear();
        if (false == _isBuildingOverrideAnimation)
        {
            SetAnimationEx(_mainAnimationData);
        }
    }
}

void AnimationComponent::BeginBuildOverrideAnimation() 
{
    if (false == _isBuildingOverrideAnimation)
    {
        _isBuildingOverrideAnimation = true;
    }
    else
    {
        assert(false && "BeginBuildOverrideAnimation called while already building an override animation");
    }
    
}

void AnimationComponent::EndBuildOverrideAnimation() 
{
    if (_isBuildingOverrideAnimation)
    {
        _isBuildingOverrideAnimation = false;
        AnimationData& animData = GetLastAnimationDataEx();
        SetAnimationEx(animData);
    }
    else
    {
        assert(false && "EndBuildOverrideAnimation called without BeginBuildOverrideAnimation");
    }
}

void AnimationComponent::PushOverrideAnimation(std::string_view animKey, bool blend, std::function<bool(const AnimationData&)> popCondition)
{
    Animator* animator = GetAnimator();
    if (animator)
    {
        _overrideAnimationStack.emplace_back(animKey);
        AnimationData& animData = GetLastAnimationDataEx();
        animData.IsBlending     = blend;
        animData.PopCondition   = popCondition;
        animData.MaxFrame       = animator->GetAnimationLastTime(animData.AnimationName.data());
        if (false == _isBuildingOverrideAnimation)
        {
            SetAnimationEx(animData);
        }
    }
}

void AnimationComponent::PopOverrideAnimation() 
{
    _overrideAnimationStack.pop_back();
    AnimationData& animData = GetLastAnimationDataEx();
    if (false == _isBuildingOverrideAnimation)
    {
        SetAnimationEx(animData);
    }
}

void AnimationComponent::ChangeCurrentAnimation(std::string_view animKey, bool blend) 
{
    ChangeAnimationEx(GetLastAnimationDataEx(), animKey, blend);
}

void AnimationComponent::ChangeMainAnimation(std::string_view animKey, bool blend) 
{
    ChangeAnimationEx(_mainAnimationData, animKey, blend);
}

void AnimationComponent::ChangeCurrentAnimationFrame(float frame) 
{
    ChangeAnimationFrameEx(GetLastAnimationDataEx(), frame);
}

void AnimationComponent::ChangeMainAnimationFrame(float frame) 
{
    ChangeAnimationFrameEx(_mainAnimationData, frame);
}

void AnimationComponent::ChangeCurrentAnimationFlags(int flags) 
{
    ChangeAnimationFlagsEx(GetLastAnimationDataEx(), flags);
}

void AnimationComponent::ChangeMainAnimationFlags(int flags) 
{
    ChangeAnimationFlagsEx(_mainAnimationData, flags);
}

const AnimationData& AnimationComponent::GetMainAnimationData() const
{
    return _mainAnimationData;
}

const AnimationData& AnimationComponent::GetLastAnimationData() const
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.back();
    }
    return _mainAnimationData;
}

void AnimationComponent::PlayCurrentAnimation() 
{
    AnimationData& animData = GetLastAnimationDataEx();
    animData.RemoveFlag(ANIMATION_FLAG_PAUSE);
    ChangeAnimationFrameEx(animData, 0.0f);
}

void AnimationComponent::ResumeCurrentAnimation() 
{
    AnimationData& animData = GetLastAnimationDataEx();
    animData.RemoveFlag(ANIMATION_FLAG_PAUSE);
}

void AnimationComponent::StopCurrentAnimation() 
{
    AnimationData& animData = GetLastAnimationDataEx();
    animData.AddFlag(ANIMATION_FLAG_PAUSE);
    ChangeAnimationFrameEx(animData, 0.0f);
}

void AnimationComponent::PauseCurrentAnimation() 
{
    AnimationData& animData = GetLastAnimationDataEx();
    animData.AddFlag(ANIMATION_FLAG_PAUSE);
}

