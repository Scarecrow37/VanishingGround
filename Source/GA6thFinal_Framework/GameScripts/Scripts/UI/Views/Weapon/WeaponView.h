#pragma once
#include "ViewModels/Weapon/WeaponViewModel.h"
#include "Utility/SingletonHelper.h"

class TextElement;
class ImageElement;
class DescriptionPanel;

class WeaponView : public Component
{
    USING_PROPERTY(WeaponView)

public:
    WeaponView();
    ~WeaponView() override;

public:
    void Focus(bool value);

protected:
    void Awake() override;
    void Start() override;

private:
    void FindElements();
    void FindBackgroundUI();
    void FindTextInfoUI();
    void FindDiscriptionUI();
    void FindIconUI();
    void FindNameUI();

    struct BackgroundUI
    {
        GameObject*   BackGroundPanel = nullptr;
        ImageElement* ImageOn         = nullptr;
        ImageElement* ImageOff        = nullptr;
    };

    struct TextInfoUI
    {
        GameObject*  TextInfoPanel = nullptr;
        TextElement* Damage        = nullptr;
        TextElement* Critical      = nullptr;
        TextElement* AttackCount   = nullptr;
        TextElement* Speed         = nullptr;
    };

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(WeaponView)

private:
    BackgroundUI                   _backgroundUI;
    TextInfoUI                     _textInfoUI;
    DescriptionPanel*              _descriptionUI;
    ImageElement*                  _iconUI;
    TextElement*                   _nameUI;

    SingletonComponent<WeaponView> _singletonComponent;
    WeaponViewModel::Handle        _watchHandle;
};
