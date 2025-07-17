#include "pch.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "Animator.h"

MeshRenderer::MeshRenderer(MeshRenderType type, const Matrix& worldMatrix)
    : _type(type)
    , _worldMatrix(worldMatrix)
    , _customDepth(0)
{
}

MeshRenderer::~MeshRenderer()
{
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
        _type = MeshRenderType::SKELETAL;
    else
        _type = MeshRenderType::STATIC;
}

void MeshRenderer::SetAnimator(std::shared_ptr<Animator> animator)
{
    _animator = animator;
}