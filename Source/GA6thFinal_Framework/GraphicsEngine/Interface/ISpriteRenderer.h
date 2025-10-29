#pragma once
#include "IGraphicsBase.h"

class ISpriteRenderer : public IGraphicsBase
{
protected:
    ISpriteRenderer()          = default;
    virtual ~ISpriteRenderer() = default;

public:
    virtual const SIZE& GetSize() const        = 0;
    virtual const SIZE& GetOriginSize() const  = 0;
    virtual const UINT  GetNumColumn() const   = 0;
    virtual const UINT  GetNumRow() const      = 0;
    virtual const UINT  GetColumnIndex() const = 0;
    virtual const UINT  GetRowIndex() const    = 0;
    virtual const float GetAlpha() const       = 0;

public:
    virtual void SetType(UIType type)                           = 0;
    virtual void SetSize(SIZE size)                             = 0;
    virtual void SetTexture(std::shared_ptr<Texture> texture)   = 0;
    virtual void SetLinearFill(float fill)                      = 0;
    virtual void SetAlpha(float alpha)                          = 0;
    virtual void SetAtlas(UINT column, UINT row)                = 0;
    virtual void SetAtlasIndex(UINT columnIndex, UINT rowIndex) = 0;
};