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

class ImageElement;
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
    void FindElements();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterHpImageView)

private:
    ImageElement*                _hpImageElement;    
    CharacterHPViewModel::Handle _watchHandle;
    std::string                  _key;
};

class ReduceGage;
class MonsterHpReduceImageView : public Component
{
    USING_PROPERTY(MonsterHpReduceImageView)

public:
    MonsterHpReduceImageView();
    void OnDestroy() override;

public:
    void Watch(const std::string& key);
    void Disable() const;

protected:
    void Awake() override;

private:
    void FindElements();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterHpReduceImageView)

private:
    ImageElement*                _reduceHpImageElement;
    ReduceGage*                  _reduceGage;
    CharacterHPViewModel::Handle _watchHandle;
    std::string                  _key;
};