#include "pchScripts.h"
#include "MonsterHpView.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"

UMREAL_COMPONENT(MonsterHpTextView)
UMREAL_COMPONENT(MonsterHpImageView)

MonsterHpTextView::MonsterHpTextView()
{
    _hpTextElement = nullptr;
}

void MonsterHpTextView::Watch(const std::string& key)
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
        UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterHpTextView: WatchKey is empty.");
        _key.clear();
    }
}

void MonsterHpTextView::Disable() const
{
    if (_hpTextElement)
        _hpTextElement->Enable = false;
}

void MonsterHpTextView::Awake()
{
    Component::Awake();
    FindTextElement();
    Disable();
}

void MonsterHpTextView::OnDestroy() 
{
    if (false == _key.empty())
    {
        UmWatcher.Blind<CharacterHPViewModel>(_key, _watchHandle);
    }
}

void MonsterHpTextView::FindTextElement()
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    bool              isFound        = false;
    Transform::ForeachBFS(ownerTransform, [this, &isFound](const Transform* transform) 
    {
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


MonsterHpImageView::MonsterHpImageView() 
{
    _hpImageElement = nullptr;
}

void MonsterHpImageView::OnDestroy() 
{
    if (false == _key.empty())
    {
        UmWatcher.Blind<CharacterHPViewModel>(_key, _watchHandle);
    }
}

void MonsterHpImageView::Awake() 
{
    Component::Awake();
    FindTextElement();
    Disable();
}

void MonsterHpImageView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<CharacterHPViewModel>(key, _watchHandle);
        if (_hpImageElement)
            _hpImageElement->Enable = true;
        try
        {
            _watchHandle = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(key, [this](const CharacterHP& value) 
            {
                if (_hpImageElement)
                {
                    _hpImageElement->SetLinearFill((float)value.CurrentHP / (float)value.MaxHP);
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
        UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterHpTextView: WatchKey is empty.");
        _key.clear();
    }
}

void MonsterHpImageView::Disable() const
{
    if (_hpImageElement)
        _hpImageElement->Enable = false;
}


void MonsterHpImageView::FindTextElement()
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    bool              isFound        = false;
    Transform::ForeachBFS(ownerTransform, [this, &isFound](const Transform* transform) 
    {
        if (isFound)
            return;

        if (GameObject& object = transform->gameObject; object.CompareTag("HP Bar"))
        {
            if (ImageElement* element = object.GetComponent<ImageElement>(); nullptr != element)
            {
                _hpImageElement = element;
                isFound        = true;
            }
        }
    });
}