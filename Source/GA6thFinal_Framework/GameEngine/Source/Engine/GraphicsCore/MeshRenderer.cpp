#include "pch.h"
#include "MeshRenderer.h"
#include "Model.h"

MeshRenderer::MeshRenderer(RENDER_TYPE type, const Matrix& worldMatrix)
    : _type(type)
    , _worldMatrix(worldMatrix)
    , _isActive(false)
    , _isDestroy(false)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::RegisterRenderQueue(std::string_view sceneName)
{
    _isDestroy = false;
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void MeshRenderer::LoadModel(std::wstring_view filePath)
{
    _model = UmResourceManager.LoadResource<Model>(filePath);
    _isActive = true;
}