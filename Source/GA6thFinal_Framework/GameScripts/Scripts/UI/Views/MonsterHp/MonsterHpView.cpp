#include "pchScripts.h"
#include "MonsterHpView.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"

UMREAL_COMPONENT(MonsterHpView)

MonsterHpView::MonsterHpView()
{
    _hpTextElement = nullptr;
}

void MonsterHpView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<CharacterHPViewModel>(key, _watchHandle);
        if (_hpTextElement)
            _hpTextElement->Enable = true;
        try
        {
            _watchHandle = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(key, [this](const CharacterHP& value) 
            {
                if (_hpTextElement)
                {
                    std::string hp = std::to_string(value.CurrentHP);
                    hp += "/";
                    hp += std::to_string(value.MaxHP);
                    _hpTextElement->Text = hp;
                }
            });
        }
        catch (const std::exception& e)
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterHpView: WatchKey is empty.");
    }
}

void MonsterHpView::Disable() const
{
    if (_hpTextElement)
        _hpTextElement->Enable = false;
}

void MonsterHpView::Awake()
{
    Component::Awake();
    FindTextElement();
    Disable();
}

void MonsterHpView::FindTextElement()
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    bool              isFound        = false;
    Transform::ForeachBFS(ownerTransform, [this, &isFound](const Transform* transform) {
        if (isFound)
            return;
        if (GameObject& object = transform->gameObject; object.CompareTag("HP Text"))
        {
            if (TextElement* element = object.GetComponent<TextElement>(); nullptr != element)
            {
                _hpTextElement = element;
                isFound        = true;
            }
        }
    });
}