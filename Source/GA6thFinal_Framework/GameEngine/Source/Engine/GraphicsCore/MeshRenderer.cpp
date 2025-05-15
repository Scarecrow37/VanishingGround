#include "pch.h"
#include "MeshRenderer.h"
#include "Model.h"

MeshRenderer::MeshRenderer(RENDER_TYPE type, const Matrix& worldMatrix)
    : _type(type)
    , _worldMatrix(worldMatrix)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetModel(std::shared_ptr<Model> model)
{
    _model = model;
    SetActive(true);
}

void MeshRenderer::RegisterRenderQueue(std::string_view sceneName)
{
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void MeshRenderer::LoadModel(std::wstring_view filePath)
{
    _model = UmResourceManager.LoadResource<Model>(filePath);
    SetActive(true);
}