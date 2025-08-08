#pragma once
#include "../PlacementUIComponent/PlacementUIComponent.h"

class EditablePlacementUIComponent : public PlacementUIComponent
{
    USING_PROPERTY(EditablePlacementUIComponent)

protected:
    using LineCallback = std::function<void(const POINT& start, const POINT& end)>;
    using QuadCallback = std::function<void(const POINT& leftTop, const POINT& rightTop, const POINT& rightBottom,
                                            const POINT& leftBottom)>;

public:
    EditablePlacementUIComponent();

public:
    REFLECT_PROPERTY(Point, Size)

    GETTER(POINT, Point) { return ReflectFields->Basefields.get().Point; }
    SETTER(POINT, Point)
    {
        ReflectFields->Basefields.get().Point = value;
        OnPlacementChange();
        SpreadPlacementToParent();
    }
    PROPERTY(Point)

    GETTER(SIZE, Size) { return ReflectFields->Basefields.get().Size; }
    SETTER(SIZE, Size)
    {
        ReflectFields->Basefields.get().Size = value;
        OnPlacementChange();
        SpreadPlacementToParent();
    }
    PROPERTY(Size)

protected:
    void OnDrawDebugOverride() override;
    void OnDrawDebugSelectedOverride() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void DrawQuad(const QuadCallback& callback) const;

protected:
    REFLECT_FIELDS_BEGIN(PlacementUIComponent)
    REFLECT_FIELDS_END(EditablePlacementUIComponent)

};