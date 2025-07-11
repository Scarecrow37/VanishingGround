#include "pch.h"
#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const Matrix& world, SpriteType type)
    : _worldMatrix(world)
    , _type(type)
    , _size()
{
}

SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::RegisterComponent(std::string_view sceneName)
{
    UmRenderer.RegisterRenderQueue(sceneName, this);
}

void SpriteRenderer::RegisterComponent()
{
    UmRenderer.RegisterRenderQueue(this);
}

void SpriteRenderer::LoadTexture(std::wstring_view filePath)
{
    _texture = UmResourceManager.LoadResource<Texture>(filePath);
    _size    = _texture->GetSize();
}