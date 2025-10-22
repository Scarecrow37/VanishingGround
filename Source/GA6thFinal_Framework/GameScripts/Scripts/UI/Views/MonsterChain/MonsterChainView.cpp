#include "pchScripts.h"
#include "MonsterChainView.h"
#include "UI/Elements/Text/TextElement.h"

UMREAL_COMPONENT(MonsterChainTextView)

MonsterChainTextView::MonsterChainTextView() 
{
    _chainTextElement = nullptr;
}

void MonsterChainTextView::Awake() 
{
    Component::Awake();
    FindTextElement();
    if (_chainTextElement)
    {
        _chainTextElement->Enable = false;
    }
}

void MonsterChainTextView::OnDestroy() 
{
    if (false == _key.empty())
    {
        UmWatcher.Blind<ChainCountViewModel>(_key, _watchHandle);
    }
}

void MonsterChainTextView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<ChainCountViewModel>(key, _watchHandle);
        if (_chainTextElement)
            _chainTextElement->Enable = true;
        try
        {
            _watchHandle = UmWatcher.Watch<ChainCountViewModel, int>(key, [this](const int& value) 
            {
                if (_chainTextElement)
                {
                    _chainTextElement->Text = std::to_string(value);
                }
            });
            _key = key;
        }
        catch (const std::exception& e)
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
            _key.clear();
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterChainTextView: WatchKey is empty.");
        _key.clear();
    }    
}

void MonsterChainTextView::FindTextElement()
{
    std::vector<GameObject*> objects = transform->FindBFSwithTag("Chain Text");
    for (auto& object : objects)
    {
        if (TextElement* element = object->GetComponent<TextElement>(); nullptr != element)
        {
            _chainTextElement = element;
        }
    }
}