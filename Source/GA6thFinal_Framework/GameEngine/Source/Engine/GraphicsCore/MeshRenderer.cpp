#include "pch.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "BaseMesh.h"
#include "Model.h"
#include "UnorderedAccessView.h"

MeshRenderer::MeshRenderer(MeshRenderType type, const Matrix& worldMatrix)
    : _type(type), _worldMatrix(worldMatrix), _customDepth(0)
{
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::RegisterComponent(std::string_view sceneName)
{
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void MeshRenderer::RegisterComponent()
{
    UmRenderer.RegisterRenderQueue(this);
}

std::shared_ptr<Animator> MeshRenderer::GetAnimator() const
{
    if (MeshRenderType::SKELETAL != _type)
        return nullptr;

    return _animator;
}

void MeshRenderer::SetModel(std::shared_ptr<Model> model)
{
    _model = model;

    if (model->GetAnimation())
    {
        _type              = MeshRenderType::SKELETAL;
        const auto& meshes = _model->GetMeshes();
        _skeletaMesheInstances.resize(meshes.size());
        for (size_t i = 0; i < meshes.size(); ++i)
        {
            const auto& viBuffer    = meshes[i]->GetVIBuffer();
            const UINT  vertexCount = viBuffer->_vertexCount;
            const UINT  stride      = sizeof(SkeletalMeshVertex);

            auto instance         = std::make_shared<SkeletalMeshInstance>();
            instance->VertexCount = vertexCount;

            instance->UAVBuffer = std::make_shared<UnorderedAccessView>();
            instance->UAVBuffer->InitializeForBuffer(stride, vertexCount);

            instance->VertexBufferView.BufferLocation = instance->UAVBuffer->GetGPUVirtualAddress();
            instance->VertexBufferView.SizeInBytes    = stride * vertexCount;
            instance->VertexBufferView.StrideInBytes  = stride;

            _skeletaMesheInstances[i] = instance;
        }
    }
    else
        _type = MeshRenderType::STATIC;
}

void MeshRenderer::SetAnimator(std::shared_ptr<Animator> animator)
{
    _animator = animator;
}

void MeshRenderer::LoadModel(std::wstring_view filePath)
{
    _model = UmResourceManager.LoadResource<Model>(filePath);

    if (MeshRenderType::SKELETAL == _type)
    {
        _animator = std::make_shared<Animator>();
        _animator->Initialize(_model->GetAnimation(), _model->GetSkeleton());
    }
}