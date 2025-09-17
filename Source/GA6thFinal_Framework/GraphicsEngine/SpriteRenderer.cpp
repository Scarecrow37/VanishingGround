#include "pch.h"
#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const Matrix& world, SpriteType type)
    : _worldMatrix(world)
    , _type(type)
    , _size()
    , _materialData()
    , _alpha(1.f)
    , _numColumn(1)
    , _numRow(1)
    , _columnIndex(0)
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

void SpriteRenderer::SetAtlas(UINT column, UINT row)
{
    _numColumn = std::max(column, 1u);
    _numRow    = std::max(row, 1u);
}

void SpriteRenderer::SetAtlasIndex(UINT columnIndex, UINT rowIndex)
{
    _columnIndex = std::min(columnIndex, _numColumn - 1);
    _rowIndex    = std::min(rowIndex, _numRow - 1);
}