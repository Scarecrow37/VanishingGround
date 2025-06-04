#include "pch.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "Animator.h"

MeshRenderer::MeshRenderer(MESH_RENDER_TYPE type, const Matrix& worldMatrix)
    : _type(type)
    , _worldMatrix(worldMatrix)
{
}

MeshRenderer::~MeshRenderer()
{
}

std::shared_ptr<Animator> MeshRenderer::GetAnimator() const
{
    if (MESH_RENDER_TYPE::SKELETAL != _type)
        return nullptr;

    return _animator;
}

void MeshRenderer::SetModel(std::shared_ptr<Model> model)
{
    _model = model;

    if (model->GetAnimation())
        _type = MESH_RENDER_TYPE::SKELETAL;

    SetActive(true);
}

void MeshRenderer::SetAnimator(std::shared_ptr<Animator> animator)
{
    _animator = animator;
}

void MeshRenderer::RegisterRenderQueue(std::string_view sceneName)
{
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void MeshRenderer::LoadModel(std::wstring_view filePath)
{
    _model = UmResourceManager.LoadResource<Model>(filePath);

    if (MESH_RENDER_TYPE::SKELETAL == _type)
    {
        _animator = std::make_shared<Animator>();
        _animator->Initialize(_model->GetAnimation(), _model->GetSkeleton());
    }

    SetActive(true);
}