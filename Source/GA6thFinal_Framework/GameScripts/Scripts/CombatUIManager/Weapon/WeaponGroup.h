#pragma once
#include <CombatUIManager/UIGroup.h>
class DescriptionPanel;
class ImageElement;
class TextElement;
class WeaponView;
class SpriteAnimationElement;

namespace CombatUI
{
    struct WeaponGroup : public UIGroup
    {
        struct BackGround
        {
            ImageElement*           FocusOn  = nullptr;
            SpriteAnimationElement* FocusOff = nullptr;
            inline bool   IsValid() const { return FocusOn && FocusOff; }
        };
        struct TextInfo
        {
            TextElement* Damage      = nullptr;
            TextElement* Critical    = nullptr;
            TextElement* AttackCount = nullptr;
            TextElement* Speed       = nullptr;
            inline bool  IsValid() const { return Damage && Critical && AttackCount && Speed; }
        };
        GameObject* Root = nullptr;
        WeaponView* View = nullptr;

        BackGround        Background;
        TextInfo          TextInfo;
        ImageElement*     Icon        = nullptr;
        TextElement*      Name        = nullptr;
        DescriptionPanel* Description = nullptr;

        // UIGroup을(를) 통해 상속됨
        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;
    };
} // namespace CombatUI