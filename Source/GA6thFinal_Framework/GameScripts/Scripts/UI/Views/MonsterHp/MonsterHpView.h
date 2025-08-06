#pragma once

class TextElement;

class MonsterHpView : public Component
{
    USING_PROPERTY(MonsterHpView)

public:
    MonsterHpView();

public:
    void Watch(const std::string& key) const;

protected:
    void Awake() override;

private:
    void FindTextElement();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterHpView)

private:
    TextElement* _hpTextElement;
};