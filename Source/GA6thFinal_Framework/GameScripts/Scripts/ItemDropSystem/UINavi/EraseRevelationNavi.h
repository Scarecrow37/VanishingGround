#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"

class EraseRevelationNavi : public UISFXNavigationComponent
{
    USING_PROPERTY(EraseRevelationNavi)
    friend class EraseRevelationUIManager;

public:
    EraseRevelationNavi();
    ~EraseRevelationNavi() override;

public:
    void SetItemInfo(const DropItemInfo& info) { _itemInfo = info; }

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(EraseRevelationNavi)

    void Awake() override;
    void FocusIn(FocusCallType type) override;
    void Submit() override;
    void FocusOut(FocusCallType type) override;

private:
    void SetEnableFocusImage(bool enable);

private:
    int _mySlot;
    std::weak_ptr<Component> _focusImage;
    DropItemInfo             _itemInfo;
};

