#pragma once

class ItemInfoUIManager;
class HorizontalPageUIManager;
class ImageElement;
class InventoryUIManager : public Component
{
    USING_PROPERTY(InventoryUIManager)

public:
    InventoryUIManager();
    ~InventoryUIManager() override;

public:
    void UpdateWeaponUI();
    void UpdateRevelationUI();
    void UpdateAccessoryUI();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(InventoryUIManager)

    void ImGuiDrawPropertysEvent() override;
    void Awake() override;
    void Start() override;

private:
    void FindUIElements();
    Transform* GetParentParent(Transform& tr);
    ItemInfoUIManager* _itemInfoUIManager;

    struct HorizontalPageUI
    {      
        HorizontalPageUIManager* Manager = nullptr;
        std::vector<ImageElement*> Icons;
    };

    std::vector<ImageElement*> FindIcons(Transform* tr);

    std::vector<ImageElement*> _weaponsUI;
    HorizontalPageUI           _revelationUI;
    HorizontalPageUI           _accessoryUI;
    std::vector<ImageElement*> _consumableUI;
};

