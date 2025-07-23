#include "pch.h"
#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const Matrix& world, SpriteType type)
    : _worldMatrix(world)
    , _type(type)
    , _size()
{
}

SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::SetTexture(std::shared_ptr<Texture> texture)
{
    _texture = std::move(texture);
    if (_texture)
    {
        _size = _texture->GetSize();
    }
}