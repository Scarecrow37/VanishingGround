#pragma once
#include "GraphicsBase.h"

class SpriteRenderer : public GraphicsBase
{
public:
    SpriteRenderer(const Matrix& world, SpriteType type);
    virtual ~SpriteRenderer();

public:
    const Matrix&  GetWorldMatrix() const { return _worldMatrix; }
    const SpriteType GetType() const { return _type; }
    const Texture* GetTexture() const { return _texture.get(); }

public:
    void SetType(SpriteType type) { _type = type; }

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void RegisterRenderQueue();
    void LoadTexture(std::wstring_view filePath);

private:
    std::shared_ptr<Texture> _texture;
    const Matrix&            _worldMatrix;
    SpriteType               _type;
};