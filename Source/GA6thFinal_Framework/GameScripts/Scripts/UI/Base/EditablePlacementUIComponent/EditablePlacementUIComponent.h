#pragma once
#include "../PlacementUIComponent/PlacementUIComponent.h"

class EditablePlacementUIComponent : public PlacementUIComponent
{
    USING_PROPERTY(EditablePlacementUIComponent)

public:
    EditablePlacementUIComponent();

    REFLECT_PROPERTY(Point, Size)

    GETTER(POINT, Point) { return ReflectFields->Basefields.get().Point; }
    SETTER(POINT, Point) { ReflectFields->Basefields.get().Point = value; }
    PROPERTY(Point)

    GETTER(SIZE, Size) { return ReflectFields->Basefields.get().Size; }
    SETTER(SIZE, Size) { ReflectFields->Basefields.get().Size = value; }
    PROPERTY(Size)

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(EditablePlacementUIComponent)

    void ImGuiDrawPropertysEvent() override;
};