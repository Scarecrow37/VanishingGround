#include "pchScripts.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent() 
    : 
    Component(Component::TYPE::MESH),
    Renderer(_pMeshRenderer)
{
  
}
MeshComponent::~MeshComponent()
{
    if (Renderer)
    {
        Renderer->SetDestroy();
        const std::shared_ptr<Animator>& animator = Renderer->GetAnimator();
        if (animator)
        {
            animator->SetDestroy();
        }     
    }
}

bool MeshComponent::HasModel()
{
    return nullptr != Renderer->GetModel();
}

bool MeshComponent::HasAnimator()
{
    return nullptr != Renderer->GetAnimator();
}

void MeshComponent::MakeMeshRenderer(MeshRenderType renderType, const Matrix& world)
{
    if (nullptr == _pMeshRenderer)
    {
        _pMeshRenderer.reset(new MeshRenderer(renderType, world));
        _pMeshRenderer->SetActive(&EnableInHierarchy);
        _pMeshRenderer->OnCustomDepth(PostProcess::BLOOM);
        UmGraphics.RegisterComponent("Game", _pMeshRenderer.get());

        if constexpr (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _pMeshRenderer.get());
        }
    } 
    else
    {
        assert(!"이미 MeshRenderer가 존재합니다.");
    }
}

