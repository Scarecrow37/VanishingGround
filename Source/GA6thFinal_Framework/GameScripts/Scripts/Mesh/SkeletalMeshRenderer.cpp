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
    SetCurrentAnimationLoop(ReflectFields->MainAnimationLooping);
    SetCurrentAnimationSpeed(ReflectFields->MainAnimationSpeed);
}

void SkeletalMeshRenderer::Update()
{
    UpdateAnimation();
}

void SkeletalMeshRenderer::OnDrawDebug() 
{
    UpdateAnimation();
}

void SkeletalMeshRenderer::SerializedReflectEvent() 
{
    ReflectFields->MainAnimationKey     = _mainAnimationData.AnimationName;
    ReflectFields->MainAnimationLooping = _mainAnimationData.IsLooping;
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
    _mainAnimationData.IsLooping     = ReflectFields->MainAnimationLooping;
    _mainAnimationData.Speed         = ReflectFields->MainAnimationSpeed;
    _mainAnimationData.IsPlaying     = true;
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
                        bool usePushStyleColor = curAnimData->IsPlaying;
                        if (true == usePushStyleColor)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                        if (ImGui::Button(EditorIcon::ICON_PLAY))
                            curAnimData->IsPlaying = !curAnimData->IsPlaying;
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

                    ImGui::Checkbox("Loop", &curAnimData->IsLooping);

                    float min = 0.0f;
                    float max = animator->GetCurrentAnimationLastTime();
                    float cur = animator->GetCurrentAnimationPlayTime();
                    if (ImGui::SliderFloat("Current Animation Frame", &cur, min, max))
                    {
                        SetCurrentAnimationFrame(cur);
                    }
                    if (ImGui::DragFloat("Animation Speed", &curAnimData->Speed, 0.01f))
                    {
                        SetCurrentAnimationSpeed(curAnimData->Speed);
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

void SkeletalMeshRenderer::UpdateAnimation() 
{
    auto animator = Renderer->GetAnimator();
    auto animData = GetLastAnimationDataEx();
    if (animator && animData)
    {
        animator->SetPause(!animData->IsPlaying);
        animator->SetLoop(animData->IsLooping);
        animator->SetAnimationSpeed(animData->Speed * ReflectFields->AnimationSpeedScale);
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

void SkeletalMeshRenderer::PushOverrideAnimation(std::string_view animKey, bool loop, bool blend,
                                                 std::function<bool(const AnimationData&)> popCondition)
{
    if (HasModel() && HasAnimator())
    {
        const auto& animator = Renderer->GetAnimator();
        _overrideAnimationStack.emplace_back(animKey, loop);
        AnimationData* animData = GetLastAnimationDataEx();
        if (animData)
        {
            animData->PopCondition = popCondition;
            animData->IsLooping    = loop;
            animData->IsBlending   = blend;
            if (false == _isBuildingOverrideAnimation)
            {
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

void SkeletalMeshRenderer::SetCurrentAnimationLoop(bool loop)
{
    SetAnimationLoop(GetLastAnimationDataEx(), loop);
}

void SkeletalMeshRenderer::SetMainAnimationLoop(bool loop) 
{
    SetAnimationLoop(&_mainAnimationData, loop);
}

void SkeletalMeshRenderer::SetCurrentAnimationFrame(float frame)
{
    SetAnimationFrame(GetLastAnimationDataEx(), frame);
}

void SkeletalMeshRenderer::SetMainAnimationFrame(float frame) 
{
    SetAnimationFrame(&_mainAnimationData, frame);
}

void SkeletalMeshRenderer::SetCurrentAnimationSpeed(float speed)
{
    SetAnimationSpeed(GetLastAnimationDataEx(), speed);
}

void SkeletalMeshRenderer::SetMainAnimationSpeed(float speed) 
{
    SetAnimationSpeed(&_mainAnimationData, speed);
}

void SkeletalMeshRenderer::SetCurrentAnimationBlend(bool blend) 
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->IsBlending = blend;
    }
}

void SkeletalMeshRenderer::SetMainAnimationBlend(bool blend)
{
    AnimationData* animData = &_mainAnimationData;
    if (animData)
    {
        animData->IsBlending = blend;
    }
}

void SkeletalMeshRenderer::StopCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->IsPlaying = false;
        SetCurrentAnimationFrame(0.0f);
    }
}

void SkeletalMeshRenderer::PlayCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->IsPlaying = true;
        SetCurrentAnimationFrame(0.0f);
    }
}

void SkeletalMeshRenderer::PauseCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->IsPlaying = false;
    }
}

void SkeletalMeshRenderer::ResumeCurrentAnimation()
{
    AnimationData* animData = GetLastAnimationDataEx();
    if (animData)
    {
        animData->IsPlaying = true;
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

void SkeletalMeshRenderer::SetAnimationLoop(AnimationData* animData, bool loop)
{
    const auto& animator = Renderer->GetAnimator();
    if (animator && animData)
    {
        animData->IsLooping = loop;
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

void SkeletalMeshRenderer::SetAnimationSpeed(AnimationData* animData, float speed)
{
    const auto& animator = Renderer->GetAnimator();
    if (animator && animData)
    {
        animData->Speed = std::max(speed, 0.0f);
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