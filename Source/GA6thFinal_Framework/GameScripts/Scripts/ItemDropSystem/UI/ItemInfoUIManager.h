#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class TextElement;
class DescriptionPanel;
class ImageElement;
struct WeaponStats;

class ItemInfoUIManager : public Component
{
    USING_PROPERTY(ItemInfoUIManager)
protected:
    struct Components
    {
        TextElement*      ItemName           = nullptr;
        ImageElement*     ItemIcon           = nullptr;
        DescriptionPanel* ItemDescription    = nullptr;
        DescriptionPanel* KeywordDescription = nullptr;

        TextElement* Damage      = nullptr;
        TextElement* Critical    = nullptr;
        TextElement* AttackCount = nullptr;
        TextElement* Speed       = nullptr;
    };

public:
    inline static const char* TAG = "Item Info Ui Manager";

    ItemInfoUIManager();
    ~ItemInfoUIManager() override;

public:
    /// <summary>
    /// 전달하는 아이템 정보로 UI 출력을 설정합니다.
    /// </summary>
    /// <param name="info :">출력할 아이템 정보 구조체</param>
    void SetItemInfoUI(const DropItemInfo& info);

    void SetItemName(const std::string& name);

    void SetItemIcon(const DropItemInfo& info);
    void SetItemIcon(const File::Guid& guid);

    void SetItemDescription(const DropItemInfo& info);
    void SetItemDescription(const std::string& description);

    void SetKeywordDescription(const std::string& description);

    void SetWeaponStats(const DropItemInfo& info);
    void SetWeaponStats(const WeaponStats& stats);
    void ClearWeaponStats();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ItemInfoUIManager)

protected:
    void Awake() override;
    void Start() override;

    void FindComponents();

private:
    SingletonComponent<ItemInfoUIManager> _singletonComponent{this};
    Components                            _uiComponents;
};

