#pragma once
#include "../UIComponent/UIComponent.h"

class AreaUIComponent : public UIComponent
{
    friend class Transform;
    USING_PROPERTY(AreaUIComponent)

public:
    AreaUIComponent();

    REFLECT_PROPERTY(Point, Size)

    GETTER(POINT, Point) { return _point; }
    SETTER(POINT, Point) { _point = value; }
    PROPERTY(Point)

    GETTER(SIZE, Size) { return _size; }
    SETTER(SIZE, Size) { _size = value; }
    PROPERTY(Size)

protected:
    REFLECT_FIELDS_BEGIN(UIComponent)

    REFLECT_FIELDS_END(AreaUIComponent)

    void OnDrawDebug() override;

    void OnDrawDebugSelected() override;

    void ImGuiDrawPropertysEvent() override;

    virtual void ResetArea() {};

    POINT _point;
    SIZE  _size;
};