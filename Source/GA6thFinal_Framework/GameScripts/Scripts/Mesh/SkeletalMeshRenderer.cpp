#include "pchScripts.h"
#include "SkeletalMeshRenderer.h"

#include "Engine/GraphicsCore/Model.h"
#include "Engine/GraphicsCore/Animation.h"
#include "Engine/GraphicsCore/Animator.h"

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

}

void SkeletalMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Guid;
    _guidRef        = guid;
    if (false == guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel();});
    }
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
            
            if (nullptr != animator)
            {
                if (ImGui::TreeNodeEx("Animation##details"))
                {
                    const auto& animationNames = animation->GetAnimations();
                    const char* comboLabel     = _currentAnimationKey.empty() ? "-" : _currentAnimationKey.c_str();
                    if (ImGui::BeginCombo("##Animation", comboLabel))
                    {
                        for (int i = 0; i < animationNames.size(); ++i)
                        {
                            bool isSelected = (_currentAnimationKey == animationNames[i]);
                            if (ImGui::Selectable(animationNames[i], isSelected))
                            {
                                _currentAnimationKey = animationNames[i];
                                SetCurrentAnimation(animationNames[i]);
                            }
                        }
                        ImGui::EndCombo();
                    }
                    if (true == _currentAnimationKey.empty())
                    {
                        ImGui::BeginDisabled();
                    }
                    {
                        bool usePushStyleColor = _isAnimationPlaying;
                        if (true == usePushStyleColor)
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
                        if (ImGui::Button(EditorIcon::ICON_PLAY))
                            _isAnimationPlaying = !_isAnimationPlaying;
                        if (true == usePushStyleColor)
                            ImGui::PopStyleColor();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Play");

                        ImGui::SameLine();

                        if (ImGui::Button(EditorIcon::ICON_PAUSE))
                            PauseAnimation();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Pause");

                        ImGui::SameLine();

                        if (ImGui::Button(EditorIcon::ICON_STOP))
                            StopAnimation();
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Stop");
                    }
                    if (true == _currentAnimationKey.empty())
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::Checkbox("Loop", &_isAnimationLooping);

                    float min = 0.0f;
                    float max = animator->GetCurrentAnimationLastTime();
                    float cur = animator->GetCurrentAnimationPlayTime();
                    if (ImGui::SliderFloat("Current Animation Frame", &cur, min, max))
                    {
                        SetAnimationFrame(cur);
                    }
                    if (ImGui::DragFloat("Animation Speed", &_animationSpeed, 0.01f))
                    {
                        SetAnimationSpeed(_animationSpeed);
                    }

                    ImGui::TreePop();
                }
            }
        }
        else
        {
            ImGui::Text("NULL Model");
        }
    }
}

void SkeletalMeshRenderer::UpdateAnimation() 
{
    if (HasModel() && HasAnimator())
    {
        auto animator = Renderer->GetAnimator();
        animator->SetPause(!_isAnimationPlaying);
        animator->SetLoop(_isAnimationLooping);
        animator->SetAnimationSpeed(_animationSpeed);
        _animationTime = animator->GetCurrentAnimationPlayTime();
    }
    else
    {
        _animationTime = 0.0f;
    }
}

void SkeletalMeshRenderer::SetCurrentAnimation(std::string_view animKey)
{
    _currentAnimationKey = animKey.data();
    if (HasModel() && HasAnimator())
    {
        const auto& model          = Renderer->GetModel();
        const auto& animator       = Renderer->GetAnimator();
        const auto& animation      = model->GetAnimation();
        const auto& animationNames = animation->GetAnimations();
        animator->ChangeAnimation(_currentAnimationKey.c_str());
        SetAnimationFrame(0.0f);
    }
}

void SkeletalMeshRenderer::SetAnimationFrame(float frame) 
{
    auto animator = Renderer->GetAnimator();
    if (nullptr != animator)
    {
        const float maxFrame = Renderer->GetAnimator()->GetCurrentAnimationLastTime();
        _animationTime = std::clamp(frame, 0.0f, maxFrame);
        animator->SetAnimationTime(_animationTime);
    }
}

void SkeletalMeshRenderer::SetAnimationSpeed(float speed) 
{
    auto animator = Renderer->GetAnimator();
    if (nullptr != animator)
    {
        _animationSpeed = std::clamp(speed, 0.0f, 100.0f);
        animator->SetAnimationSpeed(_animationSpeed);
    }
}

void SkeletalMeshRenderer::StopAnimation()
{
    SetAnimationFrame(0.0f);
    _isAnimationPlaying = false;
}

void SkeletalMeshRenderer::PlayAnimation()
{
    SetAnimationFrame(0.0f);
    _isAnimationPlaying = true;
}

void SkeletalMeshRenderer::PauseAnimation() 
{
    _isAnimationPlaying = false;
}

void SkeletalMeshRenderer::ResumeAnimation() 
{
    _isAnimationPlaying = true;
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
            Renderer->LoadModel(modelPath);
            auto& animation = Renderer->GetModel()->GetAnimation();
            auto& skeleton  = Renderer->GetModel()->GetSkeleton();
            if (animation != nullptr && skeleton != nullptr)
            {
                std::shared_ptr<Animator> animator(new Animator);
                animator->Initialize(animation, skeleton);
                animator->SetActive(&EnableInHierarchy);
                animator->RegisterComponent();
                Renderer->SetAnimator(animator);
            }
        }
    }
}