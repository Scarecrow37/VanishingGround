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
    AnimationComponent* animator = GetComponent<AnimationComponent>();
    if (animator)
    {
        animator->SetSkeletalMeshRenderer(this);
    } 
}

void SkeletalMeshRenderer::Awake() 
{
}

void SkeletalMeshRenderer::Update()
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
        if (nullptr == Renderer->GetModel())
        {
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
            }
        }
    }
}