#include "pch.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "Model.h"
#include "DXRSkeletalMesh.h"

MeshRenderer::MeshRenderer(MeshType type, const Vector3& position, const Vector3& scale, const Quaternion& rotation, const Matrix& world, const bool& dirtyFlag)
    : _type(type)
    , _transform{position, scale, rotation, world, dirtyFlag}
{
}

MeshRenderer::~MeshRenderer() {}

const std::vector<UINT>& MeshRenderer::GetCustomDepths()
{
    if (_customDepths.empty())
    {
        _customDepths.resize(_model->GetMeshCount(), PostProcess::BLOOM);
    }

    return _customDepths;
}

std::shared_ptr<Animator> MeshRenderer::GetAnimator() const
{
    if (SKELETAL_MESH != _type)
        return nullptr;

    return _animator;
}

void MeshRenderer::SetModel(std::shared_ptr<Model> model)
{
    _model = model;

    _customDepths.resize(_model->GetMeshCount(), PostProcess::BLOOM);

    if (model->GetAnimation())
    {
        _type = SKELETAL_MESH;
        auto& meshes = _model->GetMeshes();

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

void MeshRenderer::SetAnimator(std::shared_ptr<Animator> animator)
{
    _animator = animator;
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