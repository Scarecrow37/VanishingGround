#pragma once
#include "UI/Base/EditablePlacementUIComponent/EditablePlacementUIComponent.h"

class CenterWrapper : public EditablePlacementUIComponent
{
    USING_PROPERTY(CenterWrapper)

    enum class CenterType : unsigned char
    {
        BOTH,
        HORIZONTAL,
        VERTICAL,
    };

protected:
    void OnPlacementChange() override;
    void ImGuiDrawPropertysEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(EditablePlacementUIComponent)
    CenterType CenterType = CenterType::BOTH;
    REFLECT_FIELDS_END(CenterWrapper)
};