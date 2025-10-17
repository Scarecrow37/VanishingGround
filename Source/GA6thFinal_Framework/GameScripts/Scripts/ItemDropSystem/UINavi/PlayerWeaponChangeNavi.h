#pragma once

class ImageElement;
class PlayerWeaponChangeNavi : public UISFXNavigationComponent
{
    friend class WeaponChangeUIManager;
    USING_PROPERTY(PlayerWeaponChangeNavi)

public:
    PlayerWeaponChangeNavi() = default;
    ~PlayerWeaponChangeNavi() override = default;

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(UISFXNavigationComponent)
    REFLECT_FIELDS_END(PlayerWeaponChangeNavi)

    void Awake() override;
    void FocusIn(FocusCallType type) override;
    void Submit() override;
    void FocusOut(FocusCallType type) override;

private:
    size_t _weaponIndex = std::numeric_limits<size_t>::max();
    ImageElement* _focusImage  = nullptr;
};