#pragma once
#include "ViewModels/Hp/CharacterHPViewModel.h"

class TextElement;
class MonsterHpTextView : public Component
{
    USING_PROPERTY(MonsterHpTextView)

public:
    MonsterHpTextView();

public:
    void Watch(const std::string& key);
    void Disable() const;

protected:
    void Awake() override;
    void OnDestroy() override;

private:
    void FindTextElement();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterHpTextView)

private:
    TextElement*                 _hpTextElement;
    CharacterHPViewModel::Handle _watchHandle;
    std::string                  _key;    
};

class ReduceGage;
class ImageElement;
class MonsterHpImageView : public Component
{
    USING_PROPERTY(MonsterHpImageView)

public:
    MonsterHpImageView();
    ~MonsterHpImageView() override;

public:
    void Watch(const std::string& key);
    void Disable() const;

protected:
    void Awake() override;
    void OnDestroy() override;

private:
    void FindElements();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterHpImageView)

private:
    ImageElement*                _reduceHpImageElement{nullptr};
    ImageElement*                _hpImageElement{nullptr};
    ReduceGage*                  _reduceGage{nullptr};
    CharacterHPViewModel::Handle _handle;
    std::string                  _key;
};