#pragma once

class ImageElement; 
class InventoryItemFocusNavi : public UISFXNavigationComponent
{
    USING_PROPERTY(InventoryItemFocusNavi)

public:
    inline static constexpr const char* TAG = "Focus";

    InventoryItemFocusNavi();
    ~InventoryItemFocusNavi() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(InventoryItemFocusNavi)

    void FocusIn(FocusCallType callType) override;
    void Submit() override;
    void FocusOut(FocusCallType callType) override;

    void Awake() override;

    ImageElement* _focusImage;
};

