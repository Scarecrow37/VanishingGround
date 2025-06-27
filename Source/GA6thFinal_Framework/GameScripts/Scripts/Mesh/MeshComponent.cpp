#include "pchScripts.h"
#include "MeshComponent.h"
#include <Engine/GraphicsCore/Animator.h>

MeshComponent::MeshComponent() 
    : 
    Component(Component::TYPE::RENDER),
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
        _pMeshRenderer->RegisterRenderQueue("Editor");
        _pMeshRenderer->RegisterRenderQueue("Game");
        _pMeshRenderer->SetActive(&EnableInHierarchy);
    } 
    else
    {
        assert(!"이미 MeshRenderer가 존재합니다.");
    }
}
