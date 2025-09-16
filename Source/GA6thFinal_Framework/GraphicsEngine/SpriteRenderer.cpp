#include "pch.h"
#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const Matrix& world, SpriteType type)
    : _worldMatrix(world)
    , _type(type)
    , _size()
    , _materialData()
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

void SpriteRenderer::SetLinearFill(float fill)
{
    UIMaterialData materialData{.Type = UIMaterialType::LINEAR_FILL, .Fill = fill};
    _materialData = materialData;
}

void SpriteRenderer::SetAlpha(const float alpha)
{
    _material.Alpha = alpha;
}