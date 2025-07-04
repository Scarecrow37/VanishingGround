#include "pch.h"
#include "UIRenderer.h"

UIRenderer::UIRenderer(const Matrix& worldMatrix)
    : _worldMatrix(worldMatrix)
{
}

UIRenderer::~UIRenderer()
{
}

void UIRenderer::RegisterRenderQueue(std::string_view sceneName)
{
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void UIRenderer::RegisterRenderQueue()
{
    UmRenderer.RegisterRenderQueue(this);
}