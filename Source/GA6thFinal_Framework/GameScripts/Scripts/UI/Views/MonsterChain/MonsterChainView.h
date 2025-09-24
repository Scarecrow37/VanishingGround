#pragma once
#include "ViewModels/Chain/ChainCountViewModel.h"

class TextElement;
class MonsterChainTextView : public Component
{
public:
    USING_PROPERTY(MonsterChainTextView)

public:
    MonsterChainTextView();

public:
    void Watch(const std::string& key);

protected:
    void Awake() override;
    void OnDestroy() override;

private:
    void FindTextElement();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MonsterChainTextView)

private:
    TextElement*                _chainTextElement;
    ChainCountViewModel::Handle _watchHandle;
    std::string                 _key;

};