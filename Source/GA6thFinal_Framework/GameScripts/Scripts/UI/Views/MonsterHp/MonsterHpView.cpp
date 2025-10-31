#include "pchScripts.h"
#include "MonsterHpView.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "UI/Elements/Image/ImageElement.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"
#include "UI/Animations/ReduceGage/ReduceGage.h"

UMREAL_COMPONENT(MonsterHpTextView)
UMREAL_COMPONENT(MonsterHpImageView)
UMREAL_COMPONENT(MonsterHpReduceImageView)

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
    FindElements();
    Disable();
}

void MonsterHpImageView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        // 실제 HP
        UmWatcher.Blind<CharacterHPViewModel>(key, _watchHandle);

        if (_hpImageElement)
            _hpImageElement->Enable = true;
        try
        {
            _watchHandle = UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(key, [this](const CharacterHP& value) 
            {
                float currentRate = (float)value.CurrentHP / (float)value.MaxHP;

                if (_hpImageElement)
                {
                    _hpImageElement->SetLinearFill(currentRate);
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

void MonsterHpImageView::FindElements()
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    bool              isFound        = false;
    Transform::ForeachBFS(ownerTransform, [this, &isFound](const Transform* transform) {
        if (isFound)
            return;

        if (GameObject& object = transform->gameObject; object.CompareTag("HP Bar"))
        {
            if (ImageElement* element = object.GetComponent<ImageElement>(); nullptr != element)
            {
                _hpImageElement = element;
                isFound         = true;
            }
        }
    });
}

MonsterHpReduceImageView::MonsterHpReduceImageView()
{
    _reduceHpImageElement = nullptr;
    _reduceGage           = nullptr;
}

void MonsterHpReduceImageView::OnDestroy()
{
    if (false == _key.empty())
    {
        UmWatcher.Blind<CharacterHPViewModel>(_key, _watchHandle);
    }
}

void MonsterHpReduceImageView::Awake()
{
    Component::Awake();
    FindElements();
    Disable();
}

void MonsterHpReduceImageView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        std::string reduceKey = key + "_reduce";

        UmWatcher.Blind<CharacterHPViewModel>(reduceKey, _watchHandle);

        if (_reduceHpImageElement)
            _reduceHpImageElement->Enable = true;
        try
        {
            _watchHandle =
                UmWatcher.Watch<CharacterHPViewModel, CharacterHP>(reduceKey, [this](const CharacterHP& value) {
                float currentRate = (float)value.CurrentHP / (float)value.MaxHP;

                if (_reduceGage)
                {
                    _reduceGage->StartReduceGage(_reduceHpImageElement, currentRate);
                }
            });

            _key = reduceKey;
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

void MonsterHpReduceImageView::Disable() const
{
    if (_reduceHpImageElement)
        _reduceHpImageElement->Enable = false;
}

void MonsterHpReduceImageView::FindElements()
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    bool              isFound        = false;
    Transform::ForeachBFS(ownerTransform, [this, &isFound](const Transform* transform) {
        if (isFound)
            return;

        if (GameObject& reduceObject = transform->gameObject; reduceObject.CompareTag("Reduce HP Bar"))
        {
            if (ImageElement* element = reduceObject.GetComponent<ImageElement>(); nullptr != element)
            {
                _reduceHpImageElement = element;
                isFound               = true;
            }
        }
    });

    _reduceGage = GetComponent<ReduceGage>();
}