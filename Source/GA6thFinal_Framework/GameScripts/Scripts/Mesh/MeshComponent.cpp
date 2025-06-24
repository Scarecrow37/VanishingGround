#include "MeshComponent.h"

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
    }
}

void MeshComponent::MakeMeshRenderer(MeshRenderType renderType, const Matrix& world)
{
    if (nullptr == _pMeshRenderer)
    {
        _pMeshRenderer.reset(new MeshRenderer(renderType, world));
        _pMeshRenderer->RegisterRenderQueue("Editor");
        _pMeshRenderer->RegisterRenderQueue("Game");
    } 
    else
    {
        assert(!"이미 MeshRenderer가 존재합니다.");
    }
}
