#pragma once
#include "GraphicsBase.h"

class UIRenderer : public GraphicsBase
{
public:    

public:
    UIRenderer(const Matrix& world, UIType type);
    virtual ~UIRenderer();

public:
    const Matrix&          GetWorldMatrix() const { return _worldMatrix; }
    const UIType GetType() const { return _type; }

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void RegisterRenderQueue();

private:
    const Matrix& _worldMatrix;
    UIType        _type;
};