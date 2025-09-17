#include "pch.h"
#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const Matrix& world, SpriteType type)
    : _worldMatrix(world)
    , _type(type)
    , _size()
    , _materialData()
    , _alpha(1.f)
    , _numCulumn(1)
    , _numRow(1)
    , _culumnIndex(0)
    , _rowIndex(0)
{
}

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::SetTexture(std::shared_ptr<Texture> texture)
{
    _texture = std::move(texture);

    if (_texture)
    {
        _size   = _texture->GetSize();
        _origin = _size;
    }
}

void SpriteRenderer::SetLinearFill(float fill)
{
    UIMaterialData materialData{.Type = UIMaterialType::LINEAR_FILL, .Fill = fill};
    _materialData = materialData;
}

void SpriteRenderer::SetAlpha(const float alpha)
{
    _alpha = std::clamp(alpha, 0.f, 1.f);
}

void SpriteRenderer::SetAtlas(UINT culumn, UINT row)
{
    _numCulumn = std::max(culumn, 1u);
    _numRow    = std::max(row, 1u);
}

void SpriteRenderer::SetAtlasIndex(UINT culumnIndex, UINT rowIndex)
{
    _culumnIndex = std::min(culumnIndex, _numCulumn - 1);
    _rowIndex    = std::min(rowIndex, _numRow - 1);
}