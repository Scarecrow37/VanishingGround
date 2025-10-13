#include "pch.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "Model.h"
#include "DXRSkeletalMesh.h"

MeshRenderer::MeshRenderer() = default;

MeshRenderer::~MeshRenderer() = default;

const std::vector<UINT>& MeshRenderer::GetCustomDepths()
{
    if (_customDepths.empty())
    {
        _customDepths.resize(_model->GetMeshCount(), PostProcess::BLOOM);
    }

    return _customDepths;
}

IAnimator* MeshRenderer::GetAnimator() const
{
    if (SKELETAL_MESH != _type)
        return nullptr;

    return _animator.Get();
}

void MeshRenderer::SetActive(const bool* isActive)
{
    GraphicsBase::SetActive(isActive);
}

void MeshRenderer::SetModel(std::shared_ptr<Model> model)
{
    _model = model;

    _customDepths.resize(_model->GetMeshCount(), PostProcess::BLOOM);

    const auto& animation = _model->GetAnimation();

    if (animation)
    {
        _type = SKELETAL_MESH;
        
        _animator = new Animator;
        _animator->Initialize(animation, _model->GetSkeleton());

        const auto& meshes = _model->GetMeshes();

        for (auto& mesh : meshes)
        {
            auto dxrMesh = std::make_shared<DXRSkeletalMesh>(mesh->GetVIBuffer());
            dxrMesh->Initialize(mesh->GetVIBuffer()->_vertexCount, sizeof(StaticMeshVertex));
            _dxrSkeletalMeshes.push_back(dxrMesh);
        }
    }
    else
        _type = STATIC_MESH;
}

void MeshRenderer::SetMaterial(const UINT meshIndex, const Material& material)
{
    if (meshIndex < _materials.size())
    {
        _materials[meshIndex] = material;
    }
}

void MeshRenderer::SetMasterMaterial(const UINT meshIndex, const Material& material)
{    
    _model->SetMaterial(meshIndex, material);
}

void MeshRenderer::AddReference()
{
    GraphicsBase::AddReference();
}

void MeshRenderer::Release()
{
    GraphicsBase::Release();
}

void MeshRenderer::OnCustomDepth(UINT customDepth)
{
    for (auto& depth : _customDepths)
    {
        depth |= customDepth;
    }
}

void MeshRenderer::OnCustomDepth(UINT customDepth, UINT meshID)
{
    if (meshID >= _customDepths.size())
    {
        return;
    }

    _customDepths[meshID] |= customDepth;
}

void MeshRenderer::OffCustomDepth(UINT customDepth)
{
    for (auto& depth : _customDepths)
    {
        depth &= ~customDepth;
    }
}

void MeshRenderer::OffCustomDepth(UINT customDepth, UINT meshID)
{
    if (meshID >= _customDepths.size())
    {
        return;
    }

    _customDepths[meshID] &= ~customDepth;
}

void MeshRenderer::Initialize(const Matrix* world)
{
    _world = world;    
}