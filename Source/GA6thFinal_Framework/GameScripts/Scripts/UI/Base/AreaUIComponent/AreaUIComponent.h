#pragma once
#include "../UIComponent/UIComponent.h"

class AreaUIComponent : public UIComponent
{
    USING_PROPERTY(AreaUIComponent)

public:
    AreaUIComponent();

    REFLECT_PROPERTY(Point, Size)

    GETTER(POINT, Point) { return ReflectFields->Point; }
    SETTER(POINT, Point) { ReflectFields->Point = value; }
    PROPERTY(Point)

    GETTER(SIZE, Size) { return ReflectFields->Size; }
    SETTER(SIZE, Size) { ReflectFields->Size = value; }
    PROPERTY(Size)

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)
    POINT Point = {0,0};
    SIZE  Size = {200, 100};
    REFLECT_FIELDS_END(AreaUIComponent)

    void OnDrawDebug() override;

    void OnDrawDebugSelected() override;

    void ImGuiDrawPropertysEvent() override;

    virtual void ResetArea() {};

private:
    void DrawDebugQuad(FXMVECTOR color) const;

    POINT* _scopePoint;
    SIZE* _scopeSize;
};