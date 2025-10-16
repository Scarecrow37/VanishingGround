#include "pchScripts.h"
#include "AnimationComponent.h"
#include "Mesh/SkeletalMeshRenderer.h"
#include "GraphicsEngine/Interface/IAnimator.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"


UMREAL_COMPONENT(AnimationComponent)

void AnimationComponent::Added() 
{
    SetAnimator(GetComponent<SkeletalMeshRenderer>());
}

void AnimationComponent::Start()
{
    ChangeMainAnimation(ReflectFields->MainAnimationKey);
    ChangeMainAnimationFlags(ReflectFields->MainAnimationFlags);
    _currentAnimationData = &_mainAnimationData;
}

void AnimationComponent::Update() 
{
    //// 애니메이터가 해당 객체만 사용 중이라면 reset합니다.
    //UpdateNullAnimator();

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

void AnimationComponent::OnDestroy()
{
    // UpdateNullAnimator();
}

void AnimationComponent::OnEnable() 
{
    if (_animator)
    {
        AnimationData& animData = GetTopAnimationDataEx();
        _animator->SetPause(animData.HasFlag(ANIMATION_FLAG_PAUSE));
    }
}

void AnimationComponent::OnDisable() 
{
    if (_animator)
    {
        _animator->SetPause(true);
    }
}

void AnimationComponent::SerializedReflectEvent()
{
    ReflectFields->MainAnimationKey   = _mainAnimationData._animationName;
    ReflectFields->MainAnimationFlags = _mainAnimationData._flag;
    ReflectFields->MainAnimationSpeed = _mainAnimationData._speed;

    ReflectFields->AnimEventTrackGuid = _Guid.string();
}

void AnimationComponent::DeserializedReflectEvent()
{
    _mainAnimationData._animationName   = ReflectFields->MainAnimationKey;
    _mainAnimationData._flag            = ReflectFields->MainAnimationFlags;
    _mainAnimationData._speed           = ReflectFields->MainAnimationSpeed;
    SetAnimationEventTrackFromGuid(ReflectFields->AnimEventTrackGuid);
}

void AnimationComponent::ImGuiDrawPropertysEvent()
{
    // ImGui::Text("animator: use_count %d", (int)_animator.use_count());
    if (nullptr == _animator)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Animator is NULL");
    }
    if (_animator)
    {
        const auto&    animationNames = _animator->GetAnimationNames();
        AnimationData& curAnimData    = GetTopAnimationDataEx();
        if (ImGui::TreeNodeEx("Current Animation##details"))
        {
            const char* comboLabel = curAnimData._animationName.empty() ? "-" : curAnimData._animationName.c_str();
            if (ImGui::BeginCombo("##Animation", comboLabel))
            {
                for (const auto& animationName : animationNames)
                {
                    bool isSelected = (curAnimData._animationName == animationName);
                    if (ImGui::Selectable(animationName, isSelected))
                    {
                        curAnimData._animationName = animationName;
                        ChangeCurrentAnimation(animationName);
                    }
                }
                ImGui::EndCombo();
            }
            if (true == curAnimData._animationName.empty())
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
            if (true == curAnimData._animationName.empty())
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
            ImGuiHelper::TextWithVerticalSeparator("Animation Frame");
            if (ImGui::SliderFloat("##Current Animation Frame", &cur, min, max))
            {
                _animator->SetAnimationTime(cur);
            }
            ImGuiHelper::TextWithVerticalSeparator("Animation Speed");
            if (ImGui::DragFloat("##Animation Speed", &curAnimData._speed, 0.01f))
            {
            }

            ImGui::Separator();
            ImGuiHelper::TextWithVerticalSeparator("Global Animation Speed Scale");
            ImGui::DragFloat("##Global Animation Speed Scale", &ReflectFields->AnimationSpeedScale, 0.01f, 0.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Animation Event Track##details"))
        {
            ImGui::Checkbox("Disable Animation Notify", &ReflectFields->DisableAnimationNotify);
            ImGui::BeginDisabled();
            std::string path = _filePath.string();
            ImGuiHelper::TextWithVerticalSeparator("Event Track Asset");
            ImGui::InputText("##path", &path, ImGuiInputTextFlags_ReadOnly);
            ImGui::EndDisabled();
            ImGuiHelper::HoveredToolTip(path.c_str());
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
                        if (extension == AnimationEventTrack::EXTENSION)
                        {
                            SetAnimationEventTrackFromPath(path);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Separator();
            if (_eventTrack.IsLoadedFile())
            {
                ImGuiHelper::TextWithVerticalSeparator("Event Track List");
                if (ImGui::BeginCombo("##Animation", _selectedEventTrack.c_str()))
                {
                    for (const auto& animationName : animationNames)
                    {
                        bool isSelected = (curAnimData._animationName == animationName);
                        if (ImGui::Selectable(animationName, isSelected))
                        {
                            _selectedEventTrack = animationName;
                        }
                    }
                    ImGui::EndCombo();
                }
                auto eventTrack = _eventTrack.GetEventTrack(_selectedEventTrack);
                if (eventTrack)
                {
                    const auto& contextQueue = eventTrack->GetEventContextQueue();
                    if (ImGui::BeginTable("ContextTable", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
                    {
                        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch, 0.15f);
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.85f);
                        ImGui::TableHeadersRow();

                        for (const auto& context : contextQueue)
                        {
                            if (context != nullptr)
                            {
                                UINT             ID    = context->ID;
                                float            time  = context->Time;
                                std::string_view label = context->Label;

                                ImGui::PushID(context);
                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                std::string timeStr = std::format("{:.3f}", time);
                                ImGui::Selectable(timeStr.c_str());
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Selectable(label.data());
                                ImGui::PopID();
                            }
                        }
                        ImGui::EndTable();
                    }
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::Text("animation event track is not found.");
                    ImGui::PopStyleColor();
                }
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                ImGui::Text("animation event track is not loaded.");
                ImGui::PopStyleColor();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Animation Mapping Keys##details"))
        {
            auto comboList = [this](std::string& anim) {
                const auto& animationNames = _animator->GetAnimationNames();
                ImVec2 availSize = ImGui::GetContentRegionAvail();
                ImGui::SetNextItemWidth(availSize.x - 60.0f);
                if (ImGuiHelper::BeginComboInput("##AnimName", &anim))
                {
                    for (int i = 0; i < animationNames.size(); ++i)
                    {
                        bool isSelected = (anim == animationNames[i]);
                        if (ImGui::Selectable(animationNames[i], isSelected))
                        {
                            anim = animationNames[i];
                        }
                    }
                    ImGui::EndCombo();
                }
            };
            if (ImGui::BeginTable("NotifieTable##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
            {
                ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableSetupColumn("Animation Name", ImGuiTableColumnFlags_WidthStretch, 0.7f);
                ImGui::TableHeadersRow();

                int seed = 0;
                for (auto& [key, anim] : ReflectFields->AnimationKeyMap)
                {
                    bool isSelected = (anim == curAnimData._animationName);
                    ImGui::PushID(seed);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    {
                        ImVec2 availSize = ImGui::GetContentRegionAvail();
                        ImGui::Selectable(key.c_str(), isSelected, 0, ImVec2(availSize.x, 0.0f));
                        ImGuiHelper::HoveredToolTip(key.c_str());
                    }

                    ImGui::TableSetColumnIndex(1);
                    {
                        comboList(anim);
                        float height = ImGui::GetItemRectSize().y;
                        ImGuiHelper::HoveredToolTip(anim.c_str());
                        ImGui::SameLine();
                        if (ImGui::Button("-", ImVec2(height,height)))
                        {
                            _delayProcess.push_back([this, key]() { RemoveAnimationMappingKey(key); });
                        }
                    }
                    ImGui::PopID();
                    ++seed;
                }
                ImGui::PushID(seed);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                static std::string newKey;
                static std::string newKeyFromAnim;
                ImVec2 availSize = ImGui::GetContentRegionAvail();
                ImGui::SetNextItemWidth(availSize.x);
                ImGui::InputTextWithHint("##NewKey", "New Key...", &newKey);

                ImGui::TableSetColumnIndex(1);
                comboList(newKeyFromAnim);
                float height = ImGui::GetItemRectSize().y;
                ImGuiHelper::HoveredToolTip(newKeyFromAnim.c_str());
                ImGui::SameLine();
                if (ImGui::Button("+", ImVec2(height, height)))
                {
                    _delayProcess.push_back([this]() {
                        AddAnimationMappingKey(newKey, newKeyFromAnim);
                        newKey.clear();
                        newKeyFromAnim.clear();
                    });
                }
                ImGui::PopID();
                ImGui::EndTable();
            }
            ImGui::TreePop();
        }

        for (auto& process : _delayProcess)
        {
            process();
        }
        _delayProcess.clear();

        if (false == UmCore->IsPlay())
        {
            // 애니메이터가 해당 객체만 사용 중이라면 reset합니다.
            UpdateNullAnimator();
            // 메인 애니메이션만
            UpdateAnimation(_mainAnimationData);
        }
    }
}

AnimationData& AnimationComponent::GetLastAnimationDataEx()
{
    if (_lastAnimationData)
    {
        return *_lastAnimationData;
    }
    return _mainAnimationData;
}

AnimationData& AnimationComponent::GetFrontAnimationDataEx()
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.front();
    }
    return _mainAnimationData;
}

AnimationData& AnimationComponent::GetTopAnimationDataEx()
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.back();
    }
    return _mainAnimationData;
}

void AnimationComponent::UpdateNullAnimator()
{
    /*if (1 >= _animator.use_count())
    {
        _animator.reset();
        ClearOverrideAnimations();
    }*/
}

void AnimationComponent::UpdateAnimation(AnimationData& animData)
{
    if (_animator)
    {
        bool isDirty         = false;
        bool  isTopData      = GetTopAnimationDataEx().IsSameData(animData);
        float animFrameScale = animData._speed * ReflectFields->AnimationSpeedScale;
        if (isTopData)
        {
            _animator->SetPause(animData.HasFlag(ANIMATION_FLAG_PAUSE));
            _animator->SetLoop(animData.HasFlag(ANIMATION_FLAG_USE_LOOP));
            _animator->SetAnimationSpeed(animFrameScale);
            animData._elapsedFrame = _animator->GetCurrentAnimationPlayTime();
            isDirty = true;
        }
        else if (animData.HasFlag(ANIMATION_FLAG_ALWAYS_UPDATE))
        {
            float maxFrame = _animator->GetAnimationLastTime(animData._animationName.data());
            float delta    = UmTime.DeltaTime();
            animData._elapsedFrame += delta * animFrameScale;
            if (animData._elapsedFrame >= maxFrame)
            {
                animData._elapsedFrame = maxFrame;
            }
            isDirty = true;
        }
        if (true == isDirty)
        {
            auto eventTrack = _eventTrack.GetEventTrack(animData._animationName);
            if (eventTrack)
            {
                ReflectFields->DisableAnimationNotify ? eventTrack->AddFlags(Timeline::EVENT_TRCK_FLAGS_NOTIFY_DISABLED)
                                                      : eventTrack->RemoveFlags(Timeline::EVENT_TRCK_FLAGS_NOTIFY_DISABLED);
                eventTrack->SetPreNotifyCallback(_preEventCallback);
                eventTrack->SetPostNotifyCallback(_postEventCallback);
                eventTrack->SetCurrentFrame(animData._elapsedFrame);
                eventTrack->Update();
            }
            if (animData._popCondition && animData._popCondition(animData))
            {
                if (animData._onPopCallback)
                {
                    animData._onPopCallback();
                }
                PopOverrideAnimation();
            }
        }
    }
}

bool AnimationComponent::SetAnimationEx(AnimationData& animData)
{
    bool result = false;
    if (_animator)
    {
        if (false == _isBuildingOverrideAnimation)
        {
            const char* animName = animData._animationName.c_str();
            bool        canBlend = animData.HasFlag(ANIMATION_FLAG_USE_LOOP);
            result = _animator->ChangeAnimation(animName, canBlend);
            if (result)
            {
                if (_currentAnimationData && _currentAnimationData->_onExitCallback)
                {
                    _currentAnimationData->_onExitCallback();
                }
                _currentAnimationData = &animData;
                if (_currentAnimationData && _currentAnimationData->_onEnterCallback)
                {
                    _currentAnimationData->_onEnterCallback();
                }
                if (animData.HasFlag(ANIMATION_FLAG_RESET_FRAME))
                {
                    animData._elapsedFrame = 0.0f;
                }
                _animator->SetAnimationTime(animData._elapsedFrame);
                _animator->SetAnimationEndCallback(animData._onEndCallback);
                _lastAnimationData = &animData;
            }
        }
    }
    return result;
}

bool AnimationComponent::ChangeAnimationEx(AnimationData& animData, std::string_view animKey)
{
    bool result = false;
    int  nextFlag            = _nextAnimationFlag.first ? _nextAnimationFlag.second : animData._flag;
    _nextAnimationFlag.first = false;
    if (_animator)
    {
        GetAnimationNameEx(animKey, animData._animationName);
        animData._maxFrame      = _animator->GetAnimationLastTime(animData._animationName.c_str());
        animData._flag          = nextFlag;
        if (false == _isBuildingOverrideAnimation)
        {
            result = SetAnimationEx(animData);
        }
        _lastAnimationData = &animData;
    }
    return result;
}

void AnimationComponent::ChangeAnimationFrameEx(AnimationData& animData, float frame)
{
    if (_animator)
    {
        animData._elapsedFrame = std::clamp(frame, 0.0f, animData._maxFrame);
        if (false == _isBuildingOverrideAnimation)
        {
            _animator->SetAnimationTime(animData._elapsedFrame);
        }
    }
}

void AnimationComponent::ChangeAnimationFlagsEx(AnimationData& animData, int flags)
{
    if (_animator)
    {
        animData.SetFlag(flags);
    }
}

void AnimationComponent::SetAnimationPopCallbackEx(AnimationData& animData, std::function<void()> callback) 
{
    if (_animator)
    {
        animData._onPopCallback = callback;
    }
}

void AnimationComponent::SetAnimationEnterCallbackEx(AnimationData& animData, std::function<void()> callback) 
{
    if (_animator)
    {
        animData._onEnterCallback = callback;
    }
}

void AnimationComponent::SetAnimationExitCallbackEx(AnimationData& animData, std::function<void()> callback)
{
    if (_animator)
    {
        animData._onExitCallback = callback;
    }
}

void AnimationComponent::SetAnimationEndCallbackEx(AnimationData& animData, std::function<void()> callback) 
{
    if (_animator)
    {
        animData._onEndCallback = callback;
    }
}

void AnimationComponent::GetAnimationNameEx(std::string_view key, std::string& str) const
{
    bool hasKey = HasAnimationMappingKey(key);
    if (hasKey)
    {
        str = GetAnimationNameFromKey(key);
    }
}

void AnimationComponent::SetNextAnimationFlags(AnimationFlags nextAnimFlag)
{
    _nextAnimationFlag.first  = true;
    _nextAnimationFlag.second = nextAnimFlag;
}

void AnimationComponent::ClearOverrideAnimations()
{
    if (false == _overrideAnimationStack.empty())
    {
        _overrideAnimationStack.clear();
        _currentAnimationData = &GetTopAnimationDataEx();
        if (false == _isBuildingOverrideAnimation)
        {
            SetAnimationEx(_mainAnimationData);
        }
    }
    _currentAnimationData = &_mainAnimationData;
}

void AnimationComponent::BeginBuildOverrideAnimation() 
{
    if (false == _isBuildingOverrideAnimation)
    {
        _isBuildingOverrideAnimation = true;
        // 비어있으면 널
        if (_overrideAnimationStack.empty())
        {
            _prevBeginBuildAnimatonData = nullptr;
        }
        else
        {
            _prevBeginBuildAnimatonData = &GetTopAnimationData();
        }
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
        // 마지막 애니메이션 데이터가 달라졌을때만
        if (_prevBeginBuildAnimatonData != &GetTopAnimationData())
        {
            AnimationData& animData = GetTopAnimationDataEx();
            SetAnimationEx(animData);
        }
        _prevBeginBuildAnimatonData = nullptr;
    }
    else
    {
        assert(false && "EndBuildOverrideAnimation called without BeginBuildOverrideAnimation");
    }
}

bool AnimationComponent::PushBackOverrideAnimation(std::string_view animKey, bool allowOverlap)
{
    int nextFlag = _nextAnimationFlag.first ? _nextAnimationFlag.second : ANIMATION_FLAG_NONE;
    _nextAnimationFlag.first = false;
    if (_animator)
    {
        std::string animName(animKey);
        GetAnimationNameEx(animKey, animName);
        // 중복 애니메이션을 허용하지 않는 경우
        if (false == allowOverlap)
        {
            const AnimationData& topData = GetTopAnimationData();
            if (animName == topData._animationName)
            {
                return false;
            }
        }
        if (_animator->HasAnimation(animName.c_str()))
        {
            AnimationData& topData = GetTopAnimationDataEx();
            _overrideAnimationStack.emplace_back(animName);
            AnimationData& animData = _overrideAnimationStack.back();
            animData._maxFrame      = _animator->GetAnimationLastTime(animData._animationName.data());
            animData._flag          = nextFlag;
            if (false == _isBuildingOverrideAnimation)
            {
                SetAnimationEx(animData);
            }
            _lastAnimationData = &animData;
            return true;
        }
    }
    return false;
}

bool AnimationComponent::PushFrontOverrideAnimation(std::string_view animKey, bool allowOverlap)
{
    int nextFlag = _nextAnimationFlag.first ? _nextAnimationFlag.second : ANIMATION_FLAG_NONE;
    _nextAnimationFlag.first = false;
    if (_animator)
    {
        // 중복 애니메이션을 허용하지 않는 경우
        if (false == allowOverlap)
        {
            const AnimationData& frontData = GetFrontAnimationData();
            if (frontData._animationName == animKey)
            {
                return false;
            }
        }
        std::string animName(animKey);
        GetAnimationNameEx(animKey, animName);
        if (_animator->HasAnimation(animName.c_str()))
        {
            AnimationData& topData = GetTopAnimationDataEx();
            _overrideAnimationStack.emplace_front(animName);
            AnimationData& animData = _overrideAnimationStack.front();
            animData._maxFrame      = _animator->GetAnimationLastTime(animData._animationName.data());
            animData._flag          = nextFlag;
            if (false == _isBuildingOverrideAnimation)
            {
                SetAnimationEx(animData);
            }
            _lastAnimationData = &animData;
            return true;
        }
    }
    return false;
}

void AnimationComponent::PopOverrideAnimation() 
{
    if (_overrideAnimationStack.empty())
    {
        return;
    }

    if (_currentAnimationData && _currentAnimationData->_onExitCallback)
    {
        _currentAnimationData->_onExitCallback();
    }
    _currentAnimationData = nullptr;

    _overrideAnimationStack.pop_back();
    AnimationData& nextData = GetTopAnimationDataEx();
    if (false == _isBuildingOverrideAnimation)
    {
        SetAnimationEx(nextData);
    }
    _lastAnimationData = &nextData;
}

bool AnimationComponent::ChangeCurrentAnimation(std::string_view animKey) 
{
    return ChangeAnimationEx(GetLastAnimationDataEx(), animKey);
}

bool AnimationComponent::ChangeMainAnimation(std::string_view animKey) 
{
    return ChangeAnimationEx(_mainAnimationData, animKey);
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

void AnimationComponent::SetCurrentAnimationEndCallback(std::function<void()> callback) 
{
    SetAnimationEndCallbackEx(GetLastAnimationDataEx(), callback);
}

void AnimationComponent::SetMainAnimationEndCallback(std::function<void()> callback) 
{
    SetAnimationEndCallbackEx(_mainAnimationData, callback);
}

void AnimationComponent::SetCurrentAnimationEnterCallback(std::function<void()> callback) 
{
    SetAnimationEnterCallbackEx(GetLastAnimationDataEx(), callback);
}

void AnimationComponent::SetMainAnimationEnterCallback(std::function<void()> callback) 
{
    SetAnimationEnterCallbackEx(_mainAnimationData, callback);
}

void AnimationComponent::SetCurrentAnimationExitCallback(std::function<void()> callback)
{
    SetAnimationExitCallbackEx(GetLastAnimationDataEx(), callback);
}

void AnimationComponent::SetMainAnimationExitCallback(std::function<void()> callback)
{
    SetAnimationExitCallbackEx(_mainAnimationData, callback);
}

const AnimationData& AnimationComponent::GetMainAnimationData() const
{
    return _mainAnimationData;
}

void AnimationComponent::SetCurrentAnimationPopCondition(std::function<bool(const AnimationData&)> callback)
{
    AnimationData& animData = GetLastAnimationDataEx();
    animData._popCondition  = callback;
}

const AnimationData& AnimationComponent::GetFrontAnimationData() const
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.front();
    }
    return _mainAnimationData;
}

const AnimationData& AnimationComponent::GetTopAnimationData() const
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.back();
    }
    return _mainAnimationData;
}

const AnimationData& AnimationComponent::GetLastAnimationData() const
{
    if (_lastAnimationData)
    {
        return *_lastAnimationData;
    }
    return _mainAnimationData;
}

void AnimationComponent::PlayCurrentAnimation() 
{
    AnimationData& animData = GetTopAnimationDataEx();
    animData.RemoveFlag(ANIMATION_FLAG_PAUSE);
    ChangeAnimationFrameEx(animData, 0.0f);
}

void AnimationComponent::ResumeCurrentAnimation() 
{
    AnimationData& animData = GetTopAnimationDataEx();
    animData.RemoveFlag(ANIMATION_FLAG_PAUSE);
}

void AnimationComponent::StopCurrentAnimation() 
{
    AnimationData& animData = GetTopAnimationDataEx();
    animData.AddFlag(ANIMATION_FLAG_PAUSE);
    ChangeAnimationFrameEx(animData, 0.0f);
}

void AnimationComponent::PauseCurrentAnimation() 
{
    AnimationData& animData = GetTopAnimationDataEx();
    animData.AddFlag(ANIMATION_FLAG_PAUSE);
}

void AnimationComponent::SetAnimator(SkeletalMeshRenderer* renderer)
{
    if (renderer && renderer->Renderer)
    {
        _animator = renderer->Renderer->GetAnimator();
    }
}

void AnimationComponent::SetAnimator(GraphicsPointer<IAnimator> animator)
{
    if (_animator != animator)
    {
        _animator = animator;
        ClearOverrideAnimations();
    }
}

void AnimationComponent::SetAnimationEventTrackFromPath(const File::Path& path) 
{
    SetAnimationEventTrackFromGuid(path.ToGuid());
}

void AnimationComponent::SetAnimationEventTrackFromGuid(const File::Guid& guid) 
{
    _Guid  = guid;
    _filePath = guid;
    ReflectFields->AnimEventTrackGuid = _Guid.string();
    if (_eventTrack.LoadFile(_filePath))
    {
        const auto& table = _eventTrack.GetEventTrackTable();
        for (const auto& [_, track] : table)
        {
            if (track)
            {
                track->SetOwnerGameObject(gameObject->GetWeakPtr());
            }
        }
    }
}

void AnimationComponent::AddAnimationMappingKey(std::string_view key, std::string_view animKey)
{
    if (false == HasAnimationMappingKey(key))
    {
        ReflectFields->AnimationKeyMap[key.data()] = animKey;
    }
}

void AnimationComponent::RemoveAnimationMappingKey(std::string_view key) 
{
    if (true == HasAnimationMappingKey(key))
    {
        ReflectFields->AnimationKeyMap.erase(key.data());
    }
}

bool AnimationComponent::HasAnimationMappingKey(std::string_view key) const
{
    auto it = ReflectFields->AnimationKeyMap.find(key.data());
    if (it != ReflectFields->AnimationKeyMap.end())
    {
        return true;
    }
    return false;
}

const std::string& AnimationComponent::GetAnimationNameFromKey(std::string_view key) const
{
    auto it = ReflectFields->AnimationKeyMap.find(key.data());
    if (it != ReflectFields->AnimationKeyMap.end())
    {
        return it->second;
    }
    static std::string emptyString;
    return emptyString; // 기본적으로 키를 그대로 반환
}