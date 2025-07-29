#include "pchScripts.h"
#include "SkeletalMeshRenderer.h"
#include <Animation/AnimationComponent.h>

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
                        ReflectFields->Basefields.get().Guid = _guidRef.string();
                        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this](){ LoadModel(); });
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SkeletalMeshRenderer::~SkeletalMeshRenderer() 
{
}

void SkeletalMeshRenderer::Reset()
{
    MakeMeshRenderer(MeshType::SKELETAL_MESH, transform->Position, transform->Scale, transform->Rotation, transform->GetWorldMatrix());

    if (false == _guidRef.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel(); });
    } 
}

void SkeletalMeshRenderer::Awake() 
{
}

void SkeletalMeshRenderer::Update()
{
}

void SkeletalMeshRenderer::OnDestroy() 
{
}

void SkeletalMeshRenderer::OnDrawDebug() 
{
}

void SkeletalMeshRenderer::SerializedReflectEvent() 
{
}

void SkeletalMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Basefields.get().Guid;
    _guidRef        = guid;
}

void SkeletalMeshRenderer::ImGuiDrawPropertysEvent() 
{
    __super::ImGuiDrawPropertysEvent();

    if (nullptr != Renderer)
    {
        if (nullptr == Renderer->GetModel())
        {
            ImGui::Separator();
            ImGui::Text("NULL Model");
        }
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
                Renderer->OnCustomDepth(PostProcess::BLOOM);
                __super::InitMaterial();
            }
            OnChangedModel();
        }
    }
}

void SkeletalMeshRenderer::OnChangedModel() 
{
    AnimationComponent* animationComponent = GetComponent<AnimationComponent>();
    if (Renderer && animationComponent)
    {
        const auto& animator = Renderer->GetAnimator();
        animationComponent->SetAnimator(animator);
    }
}
