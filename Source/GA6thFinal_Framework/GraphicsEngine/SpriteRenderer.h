#pragma once
#include "UIRenderer.h"
#include "Interface/ISpriteRenderer.h"

class SpriteRenderer : public UIRenderer, public ISpriteRenderer
{
public:
    SpriteRenderer();
    virtual ~SpriteRenderer();

public:
    bool        IsActive() const override;
    const SIZE& GetSize() const override { return _size; }
    const SIZE& GetOriginSize() const override { return _origin; }
    const float GetAlpha() const override { return _alpha; }
    const UINT  GetNumColumn() const override { return _numColumn; }
    const UINT  GetNumRow() const override { return _numRow; }
    const UINT  GetColumnIndex() const override { return _columnIndex; }
    const UINT  GetRowIndex() const override { return _rowIndex; }

    const Matrix&         GetWorldMatrix() const { return *_worldMatrix; }    
    const Texture*        GetTexture() const { return _texture.get(); }
    const UIMaterialData& GetMaterialData() const { return _materialData; }

public:
    void SetActive(const bool* isActive) override;
    void SetType(UIType type) override { _uiType = type; }
    void SetSize(SIZE size) override { _size = size; }
    void SetTexture(std::shared_ptr<Texture> texture) override;
    void SetLinearFill(float fill) override;
    void SetRadialFill(float fill) override;
    void SetAlpha(float alpha) override;
    void SetAtlas(UINT column, UINT row) override;
    void SetAtlasIndex(UINT columnIndex, UINT rowIndex) override;

public:
    void AddReference() override;
    void Release() override;

public:
    void Initialize(const Matrix* worldMatrix);

private:
    std::shared_ptr<Texture> _texture;
    const Matrix*            _worldMatrix;
    SIZE                     _size;
    SIZE                     _origin;
    UIMaterialData           _materialData;
    UINT                     _numColumn;
    UINT                     _numRow;
    UINT                     _columnIndex;
    UINT                     _rowIndex;
    float                    _alpha;
};