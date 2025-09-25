#pragma once
#include "ViewModels/Hp/CharacterHPViewModel.h"

class TextElement;
class ImageElement;

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
    TextElement* _hpTextElement;
    CharacterHPViewModel::Handle _watchHandle;
    std::string                  _key;
};

class MonsterHpImageView : public Component
{
    USING_PROPERTY(MonsterHpImageView)

public:
    MonsterHpImageView();
    void OnDestroy() override;

public:
    void Watch(const std::string& key);
    void Disable() const;

protected:
    void Awake() override;

private:
    void FindTextElement();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterHpImageView)

private:
    ImageElement*                _hpImageElement;
    CharacterHPViewModel::Handle _watchHandle;
    std::string                  _key;
};