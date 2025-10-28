#pragma once
#include "ViewModels/Weapon/WeaponViewModel.h"
#include "Utility/SingletonHelper.h"

class TextElement;
class ImageElement;
class DescriptionPanel;
class SpriteAnimationElement;

class WeaponView : public Component
{
    USING_PROPERTY(WeaponView)

public:
    struct BackgroundUI
    {
        GameObject*             BackGroundPanel = nullptr;
        ImageElement*           FocusOn         = nullptr;
        SpriteAnimationElement* FocusOff        = nullptr;
    };

    struct TextInfoUI
    {
        GameObject*  TextInfoPanel = nullptr;
        TextElement* Damage        = nullptr;
        TextElement* Critical      = nullptr;
        TextElement* AttackCount   = nullptr;
        TextElement* Speed         = nullptr;
    };
    WeaponView();
    ~WeaponView() override;

public:
    void Focus(bool value);

    REFLECT_PROPERTY()
    GETTER_ONLY(const BackgroundUI&, BackgroundUIInfo) { return _backgroundUI; }
    PROPERTY(BackgroundUIInfo)
    GETTER_ONLY(const TextInfoUI&, TextInfo) { return _textInfoUI; }
    PROPERTY(TextInfo)
    GETTER_ONLY(DescriptionPanel*, DescriptionUI) { return _descriptionUI; }
    PROPERTY(DescriptionUI)
    GETTER_ONLY(ImageElement*, IconUI) { return _iconUI; }
    PROPERTY(IconUI)
    GETTER_ONLY(TextElement*, NameUI) { return _nameUI; }
    PROPERTY(NameUI)

protected:
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

private:
    void FindElements();
    void FindBackgroundUI();
    void FindTextInfoUI();
    void FindDiscriptionUI();
    void FindIconUI();
    void FindNameUI();

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
