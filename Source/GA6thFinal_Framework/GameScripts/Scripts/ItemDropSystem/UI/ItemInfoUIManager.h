#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"
#include "Utility/SingletonHelper.h"

class TextElement;
class DescriptionPanel;
class ImageElement;
struct WeaponStats;
class TooltipDescriptionPanel;

class ItemInfoUIManager : public Component
{
    USING_PROPERTY(ItemInfoUIManager)
protected:
    struct Components
    {
        ImageElement*            FrameImage      = nullptr;
        TextElement*             ItemName        = nullptr;
        ImageElement*            ItemIcon        = nullptr;
        DescriptionPanel*        ItemDescription = nullptr;
        DescriptionPanel*        ItemFlavor      = nullptr;
        TooltipDescriptionPanel* ItemKeyword     = nullptr;

        TextElement* Damage      = nullptr;
        TextElement* Critical    = nullptr;
        TextElement* AttackCount = nullptr;
        TextElement* Speed       = nullptr;
    };

public:
    inline static const char* TAG = "Item Info Ui Manager";

    inline File::Guid GetFrameGuid(ArtifactDropType dropType) 
    {
        switch (dropType)
        {
        case ArtifactDropType::SWORD:
        case ArtifactDropType::DAGGER:
        case ArtifactDropType::WARHAMMER:
            return File::Guid("88b9814f-6b35-45a7-9aa3-130401b36674");
        case ArtifactDropType::ACCESSORY:
        case ArtifactDropType::REVELATION:
        case ArtifactDropType::ERASE_REVELATION:
        case ArtifactDropType::Consumable:
            return File::Guid("b4ec7ed4-7493-48dd-a9c9-7620d1f95429");
        default:
            return File::Guid();
        }
    };

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

    void SetItemDescription(const DropItemInfo& info);
    void SetItemDescription(const std::string& description);

    void SetKeywordDescription(const DropItemInfo& info);

    void SetFlavorDescription(const std::string& description);

    void SetWeaponStats(const DropItemInfo& info);
    void SetWeaponStats(const WeaponStats& stats);
    void ClearWeaponStats();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ItemInfoUIManager)

protected:
    void Added() override;
    void FindComponents();

private:
    Components _uiComponents;

private:
    void SetItemIcon(const File::Guid& guid);
};

