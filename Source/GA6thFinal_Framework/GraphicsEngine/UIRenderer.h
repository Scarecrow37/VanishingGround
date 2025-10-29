#pragma once
#include "GraphicsBase.h"

class UIRenderer : public GraphicsBase
{
public:
    enum class Type
    {
        NONE,
        SPRITE,
        TEXT
    };

public:
    Type GetType() const { return _type; }
    const UIType GetUIType() const { return _uiType; }

protected:
    UIRenderer(Type type);
    virtual ~UIRenderer() = default;

protected:
    Type _type;
    UIType _uiType;
};