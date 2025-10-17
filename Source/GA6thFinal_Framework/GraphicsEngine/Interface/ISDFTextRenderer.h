#pragma once
#include "IGraphicsBase.h"

class ISDFTextRenderer : public IGraphicsBase
{
protected:
    ISDFTextRenderer()          = default;
    virtual ~ISDFTextRenderer() = default;

public:
    virtual Vector2 GetStringSize() const = 0;

public:
    virtual void SetText(const wchar_t* text)          = 0;
    virtual void SetFontSize(const float fontSize)     = 0;
    virtual void SetRotation(const Vector3& rotation)  = 0;
    virtual void SetPosition(const Vector3& position)  = 0;
    virtual void SetColor(const Vector4& color)        = 0;
    virtual void SetFontWeight(const float fontWeight) = 0;
};