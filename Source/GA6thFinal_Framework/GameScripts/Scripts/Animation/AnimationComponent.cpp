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

    ReflectFields->AnimEventTrackGuid = _guidRef.string();
}

void AnimationComponent::DeserializedReflectEvent()
{
    _mainAnimationData.AnimationName = ReflectFields->MainAnimationKey;
    _mainAnimationData.Flags         = ReflectFields->MainAnimationFlags;
    _mainAnimationData.Speed         = ReflectFields->MainAnimationSpeed;
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
        AnimationData& curAnimData    = GetLastAnimationDataEx();
        if (ImGui::TreeNodeEx("Current Animation##details"))
        {
            const char* comboLabel = curAnimData.AnimationName.empty() ? "-" : curAnimData.AnimationName.c_str();
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
            ImGuiHelper::TextWithVerticalSeparator("Animation Frame");
            if (ImGui::SliderFloat("##Current Animation Frame", &cur, min, max))
            {
                _animator->SetAnimationTime(cur);
            }
            ImGuiHelper::TextWithVerticalSeparator("Animation Speed");
            if (ImGui::DragFloat("##Animation Speed", &curAnimData.Speed, 0.01f))
            {
            }

            ImGui::Separator();
            ImGuiHelper::TextWithVerticalSeparator("Global Animation Speed Scale");
            ImGui::DragFloat("##Global Animation Speed Scale", &ReflectFields->AnimationSpeedScale, 0.01f, 0.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx("Animation Event Track##details"))
        {
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
                    for (int i = 0; i < animationNames.size(); ++i)
                    {
                        bool isSelected = (curAnimData.AnimationName == animationNames[i]);
                        if (ImGui::Selectable(animationNames[i], isSelected))
                        {
                            _selectedEventTrack = animationNames[i];
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
            auto addMapping = [this](std::string& anim) {
                const auto& animationNames = _animator->GetAnimationNames();
                AnimationData& curAnimData = GetLastAnimationDataEx();
                ImVec2 availSize = ImGui::GetContentRegionAvail();
                ImGui::SetNextItemWidth(availSize.x - 60.0f);
                if (ImGuiHelper::BeginComboInput("##AnimName", &anim))
                {
                    for (int i = 0; i < animationNames.size(); ++i)
                    {
                        bool isSelected = (curAnimData.AnimationName == animationNames[i]);
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
                    bool isSelected = (anim == curAnimData.AnimationName);
                    ImGui::PushID(seed);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    {
                        ImVec2 availSize = ImGui::GetContentRegionAvail();
                        ImGui::Selectable(key.c_str(), isSelected, 0, availSize);
                        ImGui::Text(key.c_str());
                        ImGuiHelper::HoveredToolTip(key.c_str());
                    }

                    ImGui::TableSetColumnIndex(1);
                    {
                        addMapping(anim);
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
                addMapping(newKeyFromAnim);
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

        // 애니메이터가 해당 객체만 사용 중이라면 reset합니다.
        UpdateNullAnimator();
        // 메인 애니메이션만
        UpdateAnimation(_mainAnimationData);
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
        bool isDirty = false;
        bool isLastData = GetLastAnimationDataEx().IsSameData(animData);
        float animFrameScale = animData.Speed * ReflectFields->AnimationSpeedScale;
        if (isLastData)
        {
            _animator->SetPause(animData.HasFlag(ANIMATION_FLAG_PAUSE));
            _animator->SetLoop(animData.HasFlag(ANIMATION_FLAG_USE_LOOP));
            _animator->SetAnimationSpeed(animFrameScale);
            animData.ElapsedFrame = _animator->GetCurrentAnimationPlayTime();
            isDirty = true;
        }
        else if (animData.HasFlag(ANIMATION_FLAG_ALWAYS_UPDATE))
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
                    auto itr =
                        std::remove_if(beginItr, endItr, [id](const AnimationData& data) { return data.IsSameID(id); });
                    _overrideAnimationStack.erase(itr, endItr);
                });
            }
            isDirty = true;
        }
        if (true == isDirty)
        {
            auto eventTrack = _eventTrack.GetEventTrack(animData.AnimationName);
            if (eventTrack)
            {
                eventTrack->SetPreNotifyCallback(_preEventCallback);
                eventTrack->SetPostNotifyCallback(_postEventCallback);
                eventTrack->SetCurrentFrame(animData.ElapsedFrame);
                eventTrack->Update();
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
        GetAnimationNameEx(animKey, animData.AnimationName);
        animData.IsBlending    = blend;
        animData.MaxFrame      = _animator->GetAnimationLastTime(animData.AnimationName.c_str());
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

void AnimationComponent::GetAnimationNameEx(std::string_view key, std::string& str) const
{
    bool hasKey = HasAnimationMappingKey(key);
    if (hasKey)
    {
        str = GetAnimationNameFromKey(key);
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

bool AnimationComponent::PushOverrideAnimation(std::string_view animKey, bool blend, std::function<bool(const AnimationData&)> popCondition)
{
    if (_animator)
    {
        std::string animName(animKey);
        GetAnimationNameEx(animKey, animName);
        if (_animator->HasAnimation(animKey.data()))
        {
            _overrideAnimationStack.emplace_back(animName);
            AnimationData& animData = GetLastAnimationDataEx();
            animData.IsBlending     = blend;
            animData.PopCondition   = popCondition;
            animData.MaxFrame       = _animator->GetAnimationLastTime(animData.AnimationName.data());
            if (false == _isBuildingOverrideAnimation)
            {
                SetAnimationEx(animData);
            }
            return true;
        }
    }
    return false;
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

void AnimationComponent::SetAnimationEventTrackFromPath(const File::Path& path) 
{
    _guidRef  = path;
    _filePath = path;
    _eventTrack.LoadFile(_filePath);
    ReflectFields->AnimEventTrackGuid = _guidRef.string();
}

void AnimationComponent::SetAnimationEventTrackFromGuid(const File::Guid& guid) 
{
    _guidRef  = guid;
    _filePath = guid;
    _eventTrack.LoadFile(_filePath);
    ReflectFields->AnimEventTrackGuid = _guidRef.string();
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