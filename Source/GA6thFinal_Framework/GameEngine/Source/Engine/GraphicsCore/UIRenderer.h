#pragma once
#include "GraphicsBase.h"

class UIRenderer : public GraphicsBase
{
public:
    UIRenderer(const Matrix& world);
    virtual ~UIRenderer();

public:
    const Matrix& GetWorldMatrix() const { return _worldMatrix; }

public:
    void RegisterRenderQueue(std::string_view sceneName);
    void RegisterRenderQueue();

private:
    const Matrix&             _worldMatrix;
};