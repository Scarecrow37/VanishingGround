#include "pch.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "Model.h"

MeshRenderer::MeshRenderer(MeshType type, const Vector3& position, const Vector3& scale, const Quaternion& rotation, const Matrix& world)
    : _type(type)
    , _transform{position, scale, rotation, world}
{
}

MeshRenderer::~MeshRenderer() {}

std::shared_ptr<Animator> MeshRenderer::GetAnimator() const
{
    if (SKELETAL_MESH != _type)
        return nullptr;

    return _animator;
}

void MeshRenderer::SetModel(std::shared_ptr<Model> model)
{
    _model = model;

    _customDepths.resize(_model->GetMeshCount(), 0);

    if (model->GetAnimation())
    {
        _type = SKELETAL_MESH;
        //const auto& meshes = _model->GetMeshes();
        //_skeletaMesheInstances.resize(meshes.size());
        //for (size_t i = 0; i < meshes.size(); ++i)
        //{
        //    const auto& viBuffer    = meshes[i]->GetVIBuffer();
        //    const UINT  vertexCount = viBuffer->_vertexCount;
        //    const UINT  stride      = sizeof(SkeletalMeshVertex);

        //    auto instance         = std::make_shared<SkeletalMeshInstance>();
        //    instance->VertexCount = vertexCount;

        //    instance->UAVBuffer = std::make_shared<UnorderedAccessView>();
        //    instance->UAVBuffer->InitializeForBuffer(stride, vertexCount);

        //    instance->VertexBufferView.BufferLocation = instance->UAVBuffer->GetGPUVirtualAddress();
        //    instance->VertexBufferView.SizeInBytes    = stride * vertexCount;
        //    instance->VertexBufferView.StrideInBytes  = stride;

        //    _skeletaMesheInstances[i] = instance;
        //}
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

void MeshRenderer::OffCustomDepth(UINT customDepth)
{
    for (auto& depth : _customDepths)
    {
        depth &= ~customDepth;
    }
}