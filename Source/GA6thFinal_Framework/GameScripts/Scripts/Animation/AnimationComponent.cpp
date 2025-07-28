#include "pchScripts.h"
#include "AnimationComponent.h"
#include <Mesh/SkeletalMeshRenderer.h>

void AnimationComponent::Reset() 
{
    SetAnimator(GetComponent<SkeletalMeshRenderer>());
}

void AnimationComponent::Awake()
{
    ChangeMainAnimation(ReflectFields->MainAnimationKey, false);
    ChangeMainAnimationFlags(ReflectFields->MainAnimationFlags);
}

void AnimationComponent::OnDestroy()
{
    UpdateNullAnimator();
}

void AnimationComponent::Update() 
{
    // 애니메이터가 해당 객체만 사용 중이라면 reset합니다.
    UpdateNullAnimator();
    if (_animator)
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
}

void AnimationComponent::OnDrawDebug()
{
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
    //ImGui::Text("animator: use_count %d", (int)_animator.use_count());
    if (nullptr == _animator)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Animator is NULL");
    }
    if (_animator)
    {
        const auto&    animationNames = _animator->GetAnimationNames();
        AnimationData& curAnimData    = GetLastAnimationDataEx();
        if (ImGui::TreeNodeEx("Current Animation##details"))
        {
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
            float max = _animator->GetCurrentAnimationLastTime();
            float cur = _animator->GetCurrentAnimationPlayTime();
            if (ImGui::SliderFloat("Current Animation Frame", &cur, min, max))
            {
                _animator->SetAnimationTime(cur);
            }
            if (ImGui::DragFloat("Animation Speed", &curAnimData.Speed, 0.01f))
            {
            }
            ImGui::TreePop();
        }

        // 애니메이터가 해당 객체만 사용 중이라면 reset합니다.
        UpdateNullAnimator();
        // 메인 애니메이션만
        UpdateAnimation(_mainAnimationData);

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

void AnimationComponent::UpdateAnimation(AnimationData& animData)
{
    if (_animator)
    {
        bool isLastData = GetLastAnimationDataEx().IsSameData(animData);
        float animFrameScale = animData.Speed * ReflectFields->AnimationSpeedScale;
        if (isLastData)
        {
            _animator->SetPause(animData.HasFlag(ANIMATION_FLAG_PAUSE));
            _animator->SetLoop(animData.HasFlag(ANIMATION_FLAG_USE_LOOP));
            _animator->SetAnimationSpeed(animFrameScale);
            animData.ElapsedFrame = _animator->GetCurrentAnimationPlayTime();
        }
        else
        {
            if (animData.HasFlag(ANIMATION_FLAG_ALWAYS_UPDATE))
            {
                float maxFrame = _animator->GetAnimationLastTime(animData.AnimationName.data());
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
                if (animData.OnPopCallback)
                {
                    animData.OnPopCallback();
                }
                PopOverrideAnimation();
                
            }
        }
    }
}

void AnimationComponent::SetAnimationEx(AnimationData& animData) 
{
    if (_animator)
    {
        bool isLastData = GetLastAnimationDataEx().IsSameData(animData);
        if (true == isLastData && false == _isBuildingOverrideAnimation)
        {
            _animator->ChangeAnimation(animData.AnimationName.c_str(), animData.IsBlending);
            if (animData.HasFlag(ANIMATION_FLAG_RESET_FRAME))
            {
                animData.ElapsedFrame = 0.0f;
            }
            _animator->SetAnimationTime(animData.ElapsedFrame);
        }
    }
}

void AnimationComponent::ChangeAnimationEx(AnimationData& animData, std::string_view animKey, bool blend)
{
    if (_animator)
    {
        animData.IsBlending    = blend;
        animData.AnimationName = animKey;
        animData.MaxFrame      = _animator->GetAnimationLastTime(animData.AnimationName.data());
        bool isLastData        = GetLastAnimationDataEx().IsSameData(animData);
        if (true == isLastData && false == _isBuildingOverrideAnimation)
        {
            SetAnimationEx(animData);
        }
    }
}

void AnimationComponent::ChangeAnimationFrameEx(AnimationData& animData, float frame)
{
    if (_animator)
    {
        animData.ElapsedFrame = std::clamp(frame, 0.0f, animData.MaxFrame);
        bool isLastData       = GetLastAnimationDataEx().IsSameData(animData);
        if (true == isLastData && false == _isBuildingOverrideAnimation)
        {
            _animator->SetAnimationTime(animData.ElapsedFrame);
        }
    }
}

void AnimationComponent::ChangeAnimationFlagsEx(AnimationData& animData, int flags)
{
    if (_animator)
    {
        animData.Flags = flags;
    }
}

void AnimationComponent::SetAnimationPopCallbackEx(AnimationData& animData, std::function<void()> callback) 
{
    if (_animator)
    {
        animData.OnPopCallback = callback;
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
    if (_animator)
    {
        _overrideAnimationStack.emplace_back(animKey);
        AnimationData& animData = GetLastAnimationDataEx();
        animData.IsBlending     = blend;
        animData.PopCondition   = popCondition;
        animData.MaxFrame       = _animator->GetAnimationLastTime(animData.AnimationName.data());
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

void AnimationComponent::SetCurrentAnimationPopCallback(std::function<void()> callback) 
{
    SetAnimationPopCallbackEx(GetLastAnimationDataEx(), callback);
}

void AnimationComponent::SetMainAnimationPopCallback(std::function<void()> callback) 
{
    SetAnimationPopCallbackEx(_mainAnimationData, callback);
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

void AnimationComponent::SetAnimator(SkeletalMeshRenderer* renderer)
{
    if (renderer && renderer->Renderer)
    {
        _animator = renderer->Renderer->GetAnimator();
    }
}

void AnimationComponent::SetAnimator(std::shared_ptr<Animator> animator)
{
    if (_animator != animator)
    {
        _animator = animator;
        ClearOverrideAnimations();
    }
}

void AnimationComponent::UpdateNullAnimator() 
{
    if (1 >= _animator.use_count())
    {
        _animator.reset();
        ClearOverrideAnimations();
    }
}
