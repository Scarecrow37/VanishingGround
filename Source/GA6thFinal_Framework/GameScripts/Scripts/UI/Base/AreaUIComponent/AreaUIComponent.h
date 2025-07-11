#pragma once
#include "../UIComponent/UIComponent.h"

class AreaUIComponent : public UIComponent
{
    friend class Transform;
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
};