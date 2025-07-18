#pragma once
#include "GraphicsBase.h"

class SpriteRenderer : public GraphicsBase
{
public:
    SpriteRenderer(const Matrix& world, SpriteType type);
    virtual ~SpriteRenderer();

public:
    const Matrix&    GetWorldMatrix() const { return _worldMatrix; }
    const SpriteType GetType() const { return _type; }
    const Texture*   GetTexture() const { return _texture.get(); }
    const SIZE&      GetSize() const { return _size; }

public:
    void SetType(SpriteType type) { _type = type; }
    void SetSize(SIZE size) { _size = size; }
    void SetTexture(std::shared_ptr<Texture> texture);

private:
    std::shared_ptr<Texture> _texture;
    const Matrix&            _worldMatrix;
    SpriteType               _type;
    SIZE                     _size;
};