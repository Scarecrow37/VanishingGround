#include "MeshComponent.h"

MeshComponent::MeshComponent() 
    : 
    Component(Component::Type::RENDER),
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

void MeshComponent::MakeMeshRenderer(MESH_RENDER_TYPE renderType, const Matrix& world)
{
    if (nullptr == _pMeshRenderer)
    {
        _pMeshRenderer.reset(new MeshRenderer(renderType, world));
        _pMeshRenderer->RegisterRenderQueue("Editor");
    } 
    else
    {
        assert(!"이미 MeshRenderer가 존재합니다.");
    }
}
