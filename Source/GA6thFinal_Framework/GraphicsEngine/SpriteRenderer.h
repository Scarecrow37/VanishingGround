#pragma once
#include "GraphicsBase.h"

class SpriteRenderer : public GraphicsBase
{
public:
    SpriteRenderer(const Matrix& world, SpriteType type);
    virtual ~SpriteRenderer();

public:
    const Matrix&         GetWorldMatrix() const { return _worldMatrix; }
    const SpriteType      GetType() const { return _type; }
    const Texture*        GetTexture() const { return _texture.get(); }
    const SIZE&           GetSize() const { return _size; }
    const SIZE&           GetOriginSize() const { return _origin; }    
    const float           GetAlpha() const { return _alpha; }
    const UIMaterialData& GetMaterialData() const { return _materialData; }
    const UINT            GetNumColumn() const { return _numColumn; }
    const UINT            GetNumRow() const { return _numRow; }
    const UINT            GetColumnIndex() const { return _columnIndex; }
    const UINT            GetRowIndex() const { return _rowIndex; }

public:
    void SetType(SpriteType type) { _type = type; }
    void SetSize(SIZE size) { _size = size; }
    void SetTexture(std::shared_ptr<Texture> texture);
    void SetLinearFill(float fill);
    void SetAlpha(float alpha);
    void SetAtlas(UINT column, UINT row);
    void SetAtlasIndex(UINT columnIndex, UINT rowIndex);

private:
    std::shared_ptr<Texture> _texture;
    const Matrix&            _worldMatrix;
    SIZE                     _size;
    SIZE                     _origin;
    UIMaterialData           _materialData;
    SpriteType               _type;
    UINT                     _numColumn;
    UINT                     _numRow;
    UINT                     _columnIndex;
    UINT                     _rowIndex;
    float                    _alpha;
};