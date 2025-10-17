#pragma once
#include "Utility/SingletonHelper.h"

class ItemInfoUIManager;
class HorizontalPageUIManager;
class ImageElement;
class InventoryItemFocusNavi;
class InventoryUIManager : public Component, public InputReceiver
{
    USING_PROPERTY(InventoryUIManager)

public:
    InventoryUIManager();
    ~InventoryUIManager() override;

public:
    void OpenInventory(UINavigationComponent* lastFocus = nullptr);
    void CloseInventory();

    void UpdateWeaponUI();
    void UpdateRevelationUI();
    void UpdateAccessoryUI();
    void UpdateConsumble();

    ItemInfoUIManager* GetItemInfoManager() const { return _itemInfoUIManager; }

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(InventoryUIManager)

    void ImGuiDrawPropertysEvent() override;
    void Added() override;
    void Update() override;

private:
    SingletonComponent<InventoryUIManager> _singletonComponent;

    void FindUIElements();
    ItemInfoUIManager* _itemInfoUIManager;

    struct HorizontalPageUI
    {      
        HorizontalPageUIManager* Manager = nullptr;
        std::vector<ImageElement*> Icons;
        std::vector<InventoryItemFocusNavi*> Navis;
    };

    std::vector<ImageElement*> FindIcons(Transform* tr);
    std::vector<InventoryItemFocusNavi*> FindFocus(Transform* tr);

    std::vector<ImageElement*> _weaponsUI;
    std::vector<InventoryItemFocusNavi*> _weaponsNavi;

    HorizontalPageUI           _revelationUI;
    HorizontalPageUI           _accessoryUI;

    std::vector<ImageElement*>           _consumableUI;
    std::vector<InventoryItemFocusNavi*> _consumableNavi;

    std::weak_ptr<Component>   _lastFocus; //마지막으로 포커싱된 UI

private:
    void OnButtonB(const Input::Controller&);
    bool _closeFlag = false;

private:
    size_t GetHorizontalPageCount(size_t artifactCount);

};

