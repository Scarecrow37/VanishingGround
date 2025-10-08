#include "pchScripts.h"
#include "SkeletalMeshRenderer.h"
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>

UMREAL_COMPONENT(SkeletalMeshRenderer)

SkeletalMeshRenderer::SkeletalMeshRenderer() 
{
    FilePath.SetInputAutoEvent([this]() 
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path path = data->GetPath();
                const auto extension = path.extension();
                if (extension == L".fbx" || extension == L".UmModel")
                {
                    _Guid = data->GetGuid();
                    ReflectFields->Basefields.get().Guid = _Guid.string();
                    UmSceneManager.ResourceManager.RequestModelResource(this, _Guid, [this]() { LoadModel(); });
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

SkeletalMeshRenderer::~SkeletalMeshRenderer() = default;

void SkeletalMeshRenderer::Reset()
{
    MakeMeshRenderer(MeshType::SKELETAL_MESH, transform->Position, transform->Scale, transform->Rotation, transform->GetWorldMatrix(), _isDirtyFlag);
    
    if (false == _Guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _Guid, [this]() { LoadModel(); });
    } 
}

void SkeletalMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Basefields.get().Guid;
    _Guid        = guid;
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
            Renderer->Initialize();

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
                this->InitMaterial();
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
    /*ParticleComponent* particleComponent = GetComponent<ParticleComponent>();
    if (Renderer && particleComponent)
    {
        const auto& animator = Renderer->GetAnimator();
        particleComponent->SetAnimator(animator.get());
    }*/
}
