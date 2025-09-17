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
    const UINT            GetNumCulumn() const { return _numCulumn; }
    const UINT            GetNumRow() const { return _numRow; }
    const UINT            GetCulumnIndex() const { return _culumnIndex; }
    const UINT            GetRowIndex() const { return _rowIndex; }

public:
    void SetType(SpriteType type) { _type = type; }
    void SetSize(SIZE size) { _size = size; }
    void SetTexture(std::shared_ptr<Texture> texture);
    void SetLinearFill(float fill);
    void SetAlpha(float alpha);
    void SetAtlas(UINT culumn, UINT row);
    void SetAtlasIndex(UINT culumnIndex, UINT rowIndex);

private:
    std::shared_ptr<Texture> _texture;
    const Matrix&            _worldMatrix;
    SIZE                     _size;
    SIZE                     _origin;
    UIMaterialData           _materialData;
    SpriteType               _type;
    UINT                     _numCulumn;
    UINT                     _numRow;
    UINT                     _culumnIndex;
    UINT                     _rowIndex;
    float                    _alpha;
};