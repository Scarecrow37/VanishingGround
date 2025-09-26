#pragma once
#include "IGraphicsBase.h"

class ISDFTextRenderer : public IGraphicsBase
{
protected:
    ISDFTextRenderer()          = default;
    virtual ~ISDFTextRenderer() = default;

public:
    virtual void SetText(const wchar_t* text)         = 0;
    virtual void SetPosition(const Vector3& position) = 0;
    virtual void SetScale(const float scale)          = 0;
    virtual void SetColor(const Vector4& color)       = 0;
    virtual void SetRotation(const float rotation)    = 0;

public:
    virtual void Initialize() = 0;
};