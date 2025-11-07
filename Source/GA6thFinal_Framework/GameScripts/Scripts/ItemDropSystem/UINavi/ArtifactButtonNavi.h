#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"

class InputOkCancelComponent;
class ImageElement;
class ArtifactButtonNavi : public UISFXNavigationComponent
{
    friend class ArtifactUIManager;
    friend class ItemDropUIRootManager;
    friend class ItemDropSystem;
    USING_PROPERTY(ArtifactButtonNavi)
    inline static size_t LastFocusIndex = 0;
public:
    inline static size_t GetLastFocusIndex() { return LastFocusIndex; }

    ArtifactButtonNavi();
    ~ArtifactButtonNavi() override;

    /// <summary>
    /// 버튼이 눌릴때 동작할 아이템을 설정합니다.
    /// </summary>
    /// <param name="item :">아이템 정보</param>
    void SettingItem(const DropItemInfo& item);

public:
    REFLECT_PROPERTY();

protected:
    void Awake() override;
    void FocusIn(FocusCallType type) override;
    void Submit() override;
    void FocusOut(FocusCallType type) override;
    std::shared_ptr<ImageElement> FindFocusImage();

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(ArtifactButtonNavi)

private:
    DropItemInfo _itemInfo;
    std::weak_ptr<ImageElement> _focusImage;
    size_t                      _buttonIndex = std::numeric_limits<size_t>::max();

    std::weak_ptr<InputOkCancelComponent> _revelationWarning;
    std::weak_ptr<ImageElement>           _revelationWarningIcon;

    std::weak_ptr<InputOkCancelComponent> _accessoriesWarning;
    std::weak_ptr<ImageElement>           _accessoriesWarningIcon;
};