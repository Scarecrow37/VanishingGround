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
                        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef);
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
    if (HasModel() && HasAnimator())
    {
        const auto& animator = Renderer->GetAnimator();
        if (true == _isPlaying)
        {
            _animationFrame += UmTime.DeltaTime();
            const float maxFrame = animator->GetCurrentAnimationLastTime();
            _animationFrame = std::clamp(_animationFrame, 0.0f, maxFrame);
        }
        animator->SetAnimationTime(_animationFrame);
    }
    else
    {
        _animationFrame = 0.0f;
    }
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
        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef);
    }
}

void SkeletalMeshRenderer::ImGuiDrawPropertysEvent() 
{
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
                ImGui::Text("Animation");
                ImGui::SameLine();
                if (ImGui::BeginCombo("##Animation", _currentAnimationKey.c_str()))
                {
                    for (int i = 0; i < animationNames.size(); ++i)
                    {
                        bool isSelected = strcmp(animationNames[i], _currentAnimationKey.c_str()) == 0;
                        if (ImGui::Selectable(animationNames[i], isSelected))
                        {
                            SetCurrentAnimation(animationNames[i]);
                        }
                        // 선택된 항목은 포커스를 줌
                        if (true == isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        } 
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Play"))
                {
                    PlayAnimation();
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop"))
                {
                    StopAnimation();
                }
                ImGui::SameLine();
                if (ImGui::Button("Pause"))
                {
                    PauseAnimation();
                }
                ImGui::SameLine();
                if (ImGui::Button("Resume"))
                {
                    ResumeAnimation();
                }
            }
        }
        else
        {
            ImGui::Text("NULL Model");
        }
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
    }
    else
    {
        return;
    }
}

void SkeletalMeshRenderer::SetAnimationFrame(float frame) 
{
    const float maxFrame = Renderer->GetAnimator()->GetCurrentAnimationLastTime();
    _animationFrame = std::clamp(frame, 0.0f, maxFrame);
}

void SkeletalMeshRenderer::StopAnimation()
{
    if (HasModel() && HasAnimator())
    {
        _animationFrame      = 0.0f;
        _isPlaying           = false;
    }
}

void SkeletalMeshRenderer::PlayAnimation()
{
    if (HasModel() && HasAnimator())
    {
        _animationFrame      = 0.0f;
        _isPlaying           = true;
    }
}

void SkeletalMeshRenderer::PauseAnimation() 
{
    if (HasModel() && HasAnimator())
    {
        _isPlaying = false;
    }
}

void SkeletalMeshRenderer::ResumeAnimation() 
{
    if (HasModel() && HasAnimator())
    {
        _isPlaying = true;
    }
}

