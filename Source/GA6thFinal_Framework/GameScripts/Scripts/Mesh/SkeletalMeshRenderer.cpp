#include "pchScripts.h"
#include "SkeletalMeshRenderer.h"

SkeletalMeshRenderer::SkeletalMeshRenderer() 
{
    FilePath.SetInputAutoEvent([this]() 
    {
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
                    if (extension == L".fbx" || extension == L".UmModel")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this](){ LoadModel(); });
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SkeletalMeshRenderer::~SkeletalMeshRenderer() {}

void SkeletalMeshRenderer::Reset()
{
    MakeMeshRenderer(MeshRenderType::SKELETAL, gameObject->transform->GetWorldMatrix());
}

void SkeletalMeshRenderer::Awake() 
{
    SetCurrentAnimation(ReflectFields->MainAnimationKey, false);
    SetCurrentAnimationFlags(ReflectFields->MainAnimationFlags);
}

void SkeletalMeshRenderer::Update()
{
    for (auto& animData : _overrideAnimationStack)
    {
        UpdateAnimation(&animData);
    }
    UpdateAnimation(&_mainAnimationData);

    for (auto& event : _eventQueue)
    {
        event();
    }
    _eventQueue.clear();
}

void SkeletalMeshRenderer::OnDrawDebug() 
{
    UpdateAnimation(&_mainAnimationData);
}

void SkeletalMeshRenderer::SerializedReflectEvent() 
{
    ReflectFields->MainAnimationKey     = _mainAnimationData.AnimationName;
    ReflectFields->MainAnimationFlags   = _mainAnimationData.Flags;
    ReflectFields->MainAnimationSpeed   = _mainAnimationData.Speed;
}

void SkeletalMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Guid;
    _guidRef        = guid;
    if (false == guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel();});
    }
    _mainAnimationData.AnimationName = ReflectFields->MainAnimationKey;
    _mainAnimationData.Flags         = ReflectFields->MainAnimationFlags;
    _mainAnimationData.Speed         = ReflectFields->MainAnimationSpeed;
    _mainAnimationData.Duration      = 0.0f;
}

void SkeletalMeshRenderer::ImGuiDrawPropertysEvent() 
{
    ImGui::Separator();
    if (nullptr != Renderer)
    {
        const auto& model = Renderer->GetModel();
        const auto& animator = Renderer->GetAnimator();
        if (nullptr != model)
        {
            const auto& animation      = model->GetAnimation();
            const auto& animationNames = animation->GetAnimations();
            
            AnimationData* curAnimData = GetLastAnimationDataEx();
            if (animator && curAnimData)
            {
                if (ImGui::TreeNodeEx("Current Animation##details"))
                {
                    const auto& animationNames = animation->GetAnimations();
                    const char* comboLabel     = curAnimData->AnimationName.empty() ? "-" : curAnimData->AnimationName.c_str();
                    if (ImGui::BeginCombo("##Animation", comboLabel))
                    {
                        for (int i = 0; i < animationNames.size(); ++i)
                        {
                            bool isSelected = (curAnimData->AnimationName == animationNames[i]);
                            if (ImGui::Selectable(animationNames[i], isSelected))
                            {
                                curAnimData->AnimationName = animationNames[i];
                                SetCurrentAnimation(animationNames[i]);
                            }
                        }
                        ImGui::EndCombo();
                    }
                    if (true == curAnimData->AnimationName.empty())
                    {
                        ImGui::BeginDisabled();
                    }
                    {
                        bool usePushStyleColor = !curAnimData->HasFlag(ANIMATION_FLAG_PAUSE);
                        if (true == usePushStyleColor)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                        if (ImGui::Button(EditorIcon::ICON_PLAY))
                            curAnimData->ToggleFlag(ANIMATION_FLAG_PAUSE);
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
                    if (true == curAnimData->AnimationName.empty())
                    {
                        ImGui::EndDisabled();
                    }

                    bool loop = curAnimData->HasFlag(ANIMATION_FLAG_USE_LOOP);
                    if (ImGui::Checkbox("Loop", &loop))
                    {
                        curAnimData->ToggleFlag(ANIMATION_FLAG_USE_LOOP);
                    }

                    float min = 0.0f;
                    float max = animator->GetCurrentAnimationLastTime();
                    float cur = animator->GetCurrentAnimationPlayTime();
                    if (ImGui::SliderFloat("Current Animation Frame", &cur, min, max))
                    {
                        animator->SetAnimationTime(cur);
                    }
                    if (ImGui::DragFloat("Animation Speed", &curAnimData->Speed, 0.01f))
                    {
                    }

                    ImGui::TreePop();
                }
            }
            ImGui::Separator();
            ImGuiHelper::TextWithVerticalSeparator("Animation Speed Scale");
            ImGui::DragFloat("##Animation Speed Scale", &ReflectFields->AnimationSpeedScale, 0.01f, 0.0f);
        }
        else
        {
            ImGui::Text("NULL Model");
        }
    }
}

void SkeletalMeshRenderer::UpdateAnimation(AnimationData* animData)
{
    auto animator   = Renderer->GetAnimator();
    bool isLastData = animData == GetLastAnimationDataEx();
    if (animator && animData)
    {
        float animFrameScale = animData->Speed * ReflectFields->AnimationSpeedScale;
        if (isLastData)
        {
            animator->SetPause(animData->HasFlag(ANIMATION_FLAG_PAUSE));
            animator->SetLoop(animData->HasFlag(ANIMATION_FLAG_USE_LOOP));
            animator->SetAnimationSpeed(animFrameScale);
            animData->Duration = animator->GetCurrentAnimationPlayTime();
            animData->IsEnd    = animator->IsEnd();

            if (animData->PopCondition)
            {
                bool result = animData->PopCondition(*animData);
                if (result)
                {
                    PopOverrideAnimation();
                }
            }
        }
        else
        {
            if (animData->HasFlag(ANIMATION_FLAG_ALWAYS_UPDATE))
            {
                float maxFrame = animator->GetAnimationLastTime(animData->AnimationName.data());
                float delta = UmTime.DeltaTime();
                animData->Duration += delta * animFrameScale;
                if (animData->Duration >= maxFrame)
                {
                    _eventQueue.push_back([this, name = animData->AnimationName]() {
                        auto itr = std::remove_if(_overrideAnimationStack.begin(), _overrideAnimationStack.end(),
                                           [&name](const AnimationData& data) { return data.AnimationName == name; });
                    _overrideAnimationStack.erase(itr, _overrideAnimationStack.end());
                    });
                }
            }
        }
    }
}

void SkeletalMeshRenderer::ClearOverrideAnimations() 
{
    const auto& animator = Renderer->GetAnimator();
    const auto& animData = GetLastAnimationDataEx();
    _overrideAnimationStack.clear();
    if (animator && animData)
    {
        if (false == _isBuildingOverrideAnimation)
        {
            animator->ChangeAnimation(animData->AnimationName.c_str(), animData->IsBlending);
            animator->SetAnimationTime(animData->Duration);
        }
    }
}

void SkeletalMeshRenderer::BeginBuildOverrideAnimation() 
{
    _isBuildingOverrideAnimation = true;
}

void SkeletalMeshRenderer::EndBuildOverrideAnimation() 
{
    if (_isBuildingOverrideAnimation)
    {
        const auto&    animator = Renderer->GetAnimator();
        AnimationData* animData = GetLastAnimationDataEx();
        if (animator && animData)
        {
            animator->ChangeAnimation(animData->AnimationName.c_str(), animData->IsBlending);
            animator->SetAnimationTime(animData->Duration);
        }
        _isBuildingOverrideAnimation = false;
    }
}

void SkeletalMeshRenderer::PushOverrideAnimation(std::string_view animKey, bool blend, std::function<bool(const AnimationData&)> popCondition)
{
    if (HasModel() && HasAnimator())
    {
        const auto& animator = Renderer->GetAnimator();
        _overrideAnimationStack.emplace_back(animKey);
        AnimationData* animData = GetLastAnimationDataEx();
        if (animData)
        {
            animData->PopCondition = popCondition;
            if (false == _isBuildingOverrideAnimation)
            {
                animData->IsBlending = blend;
                animator->ChangeAnimation(animData->AnimationName.c_str(), animData->IsBlending);
                animator->SetAnimationTime(animData->Duration);
            }
        }
    }
}

void SkeletalMeshRenderer::PopOverrideAnimation() 
{
    if (HasModel() && HasAnimator())
    {
        const auto& animator = Renderer->GetAnimator();
        _overrideAnimationStack.pop_back();
        AnimationData* animData = GetLastAnimationDataEx();
        if (animData)
        {
            if (false == _isBuildingOverrideAnimation)
            {
                animator->ChangeAnimation(animData->AnimationName.c_str(), animData->IsBlending);
                animator->SetAnimationTime(animData->Duration);
            }
        }
    }
}

void SkeletalMeshRenderer::SetCurrentAnimation(std::string_view animKey, bool blend)
{
    SetAnimation(GetLastAnimationDataEx(), animKey, blend);
}

void SkeletalMeshRenderer::SetMainAnimation(std::string_view animKey, bool blend) 
{
    SetAnimation(&_mainAnimationData, animKey, blend);
}

void SkeletalMeshRenderer::SetCurrentAnimationFrame(float frame)
{
    SetAnimationFrame(GetLastAnimationDataEx(), frame);
}

void SkeletalMeshRenderer::SetMainAnimationFrame(float frame) 
{
    SetAnimationFrame(&_mainAnimationData, frame);
}

void SkeletalMeshRenderer::SetCurrentAnimationFlags(int flags) 
{
    SetAnimationFlags(GetLastAnimationDataEx(), flags);
}

void SkeletalMeshRenderer::SetMainAnimationFlags(int flags) 
{
    SetAnimationFlags(&_mainAnimationData, flags);
}

void SkeletalMeshRenderer::StopCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->AddFlag(ANIMATION_FLAG_PAUSE);
        SetCurrentAnimationFrame(0.0f);
    }
}

void SkeletalMeshRenderer::PlayCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->RemoveFlag(ANIMATION_FLAG_PAUSE);
        SetCurrentAnimationFrame(0.0f);
    }
}

void SkeletalMeshRenderer::PauseCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->AddFlag(ANIMATION_FLAG_PAUSE);
    }
}

void SkeletalMeshRenderer::ResumeCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->RemoveFlag(ANIMATION_FLAG_PAUSE);
    }
}

const AnimationData& SkeletalMeshRenderer::GetMainAnimationData() const
{
    return _mainAnimationData;
}

const AnimationData& SkeletalMeshRenderer::GetLastAnimationData() const
{
    if (false == _overrideAnimationStack.empty())
    {
        return _overrideAnimationStack.back();
    }
    return _mainAnimationData;
}

AnimationData* SkeletalMeshRenderer::GetLastAnimationDataEx()
{
    if (false == _overrideAnimationStack.empty())
    {
        return &_overrideAnimationStack.back();
    }
    return &_mainAnimationData;
}

void SkeletalMeshRenderer::SetAnimation(AnimationData* animData, std::string_view animKey, bool blend) 
{
    const auto& animator = Renderer->GetAnimator();
    if (animator && animData)
    {
        animData->AnimationName = animKey;
        if (false == _isBuildingOverrideAnimation)
        {
            animator->ChangeAnimation(animData->AnimationName.c_str(), blend);
            SetCurrentAnimationFrame(animData->Duration);
        }
    }
}

void SkeletalMeshRenderer::SetAnimationFrame(AnimationData* animData, float frame)
{
    const auto&  animator = Renderer->GetAnimator();
    if (animator && animData)
    {
        const float maxFrame = animator->GetAnimationLastTime(animData->AnimationName.data());
        animData->Duration = std::clamp(frame, 0.0f, maxFrame);
    }
}

void SkeletalMeshRenderer::SetAnimationFlags(AnimationData* animData, int flags) 
{
    const auto& animator = Renderer->GetAnimator();
    if (animator && animData)
    {
        animData->Flags = flags;
    }
}

void SkeletalMeshRenderer::LoadModel()
{
    if (Renderer)
    {
        const auto& animator = Renderer->GetAnimator();
        if (animator)
        {
            animator->SetDestroy();
        }

        std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            std::wstring modelPath = U8ToWString(path);
            UmGraphics.LoadResource(modelPath, Renderer.get());
            auto& animation = Renderer->GetModel()->GetAnimation();
            auto& skeleton  = Renderer->GetModel()->GetSkeleton();
            if (animation != nullptr && skeleton != nullptr)
            {
                std::shared_ptr<Animator> animator(new Animator);
                animator->Initialize(animation, skeleton);
                animator->SetActive(&EnableInHierarchy);
                UmGraphics.RegisterComponent(animator.get());
                Renderer->SetAnimator(animator);
            }
        }
    }
}