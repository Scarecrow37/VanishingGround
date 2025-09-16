#include "pchScripts.h"
#include "MonsterHpView.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "ViewModels/HP/HpViewModel.h"

MonsterHpView::MonsterHpView() = default;

void MonsterHpView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<HpViewModel>(key, _watchHandle);
        if (_hpTextElement)
            _hpTextElement->Enable = true;
        try
        {
            _watchHandle = UmWatcher.Watch<HpViewModel, float>(key, [this](const float value) 
            {
                if (_hpTextElement)
                {
                    _hpTextElement->Text = std::to_string(static_cast<int>(value * 100)) + "%";
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