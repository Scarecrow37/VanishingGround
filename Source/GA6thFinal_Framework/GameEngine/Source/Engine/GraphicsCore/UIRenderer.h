#pragma once
#include "GraphicsBase.h"

class UIRenderer : public GraphicsBase
{
public:
    UIRenderer(const Matrix& world, UIType type);
    virtual ~UIRenderer();

public:
    const Matrix&  GetWorldMatrix() const { return _worldMatrix; }
    const UIType   GetType() const { return _type; }
    const Texture* GetTexture() const { return _texture.get(); }

public:
    void SetType(UIType type) { _type = type; }

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void RegisterRenderQueue();
    void LoadTexture(std::wstring_view filePath);

private:
    std::shared_ptr<Texture> _texture;
    const Matrix&            _worldMatrix;
    UIType                   _type;
};