#pragma once
#include "ViewModels/Hp/CharacterHPViewModel.h"

class TextElement;
class PlayerHPTextView : public Component
{
    USING_PROPERTY(PlayerHPTextView)

public:
    PlayerHPTextView();
    ~PlayerHPTextView() override;

public:
    void Start() override;
    void OnDestroy() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PlayerHPTextView)

private:
    TextElement*                 _hp{nullptr};
    CharacterHPViewModel::Handle _handle;
};

class ReduceGage;
class ImageElement;
class PlayerHPImageView : public Component
{
    USING_PROPERTY(PlayerHPImageView)

public:
    PlayerHPImageView();
    ~PlayerHPImageView() override;

protected:
    void Start() override;
    void OnDestroy() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(PlayerHPImageView)

private:
    ImageElement*                _reduceHpImageElement{nullptr};
    ImageElement*                _hpImageElement{nullptr};
    ReduceGage*                  _reduceGage{nullptr};
    CharacterHPViewModel::Handle _handle;
};