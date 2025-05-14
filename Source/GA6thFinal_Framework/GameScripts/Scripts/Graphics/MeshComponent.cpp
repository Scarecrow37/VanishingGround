#include "MeshComponent.h"

MeshComponent::MeshComponent() 
    : 
    _meshRenderer(_pMeshRenderer)
{
  
}
MeshComponent::~MeshComponent() = default;

void MeshComponent::MakeMeshRenderer(MeshRenderer::RENDER_TYPE renderType, const Matrix& world)
{
    if (nullptr == _pMeshRenderer)
    {
        _pMeshRenderer.reset(new MeshRenderer(renderType, world));
    } 
    else
    {
        assert(!"이미 MeshRenderer가 존재합니다.");
    }
}
