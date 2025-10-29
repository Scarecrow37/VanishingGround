#include "pch.h"
#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer()
    : UIRenderer(UIRenderer::Type::SPRITE)
    , _texture(nullptr)
    , _worldMatrix(nullptr)
    , _size({0, 0})
    , _origin({0, 0})
    , _materialData({.Type = UIMaterialType::DEFAULT, .Fill = 0.f})
    , _numColumn(1)
    , _numRow(1)
    , _columnIndex(0)
    , _rowIndex(0)
    , _alpha(1.f)
{
    _uiType = UIType::MODE_2D;
}

SpriteRenderer::~SpriteRenderer() = default;

bool SpriteRenderer::IsActive() const
{
    return GraphicsBase::IsActive() && _texture && _texture->IsValid();
}

void SpriteRenderer::SetActive(const bool* isActive)
{
    GraphicsBase::SetActive(isActive);
}

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

void SpriteRenderer::SetRadialFill(float fill)
{
    UIMaterialData materialData{.Type = UIMaterialType::RADIAL_FILL, .Fill = fill};
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

void SpriteRenderer::AddReference()
{
    GraphicsBase::AddReference();
}

void SpriteRenderer::Release()
{
    GraphicsBase::Release();
}

void SpriteRenderer::Initialize(const Matrix* worldMatrix)
{
    _worldMatrix = worldMatrix;
}