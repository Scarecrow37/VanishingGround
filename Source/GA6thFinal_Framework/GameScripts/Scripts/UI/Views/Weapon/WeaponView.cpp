#include "pchScripts.h"
#include "WeaponView.h"

#include "ViewModels/Weapon/WeaponViewModel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"

WeaponView::WeaponView() = default;

WeaponView::~WeaponView()
{
    UmWatcher.Blind<WeaponViewModel>("Weapon");
}

void WeaponView::Awake()
{
    Component::Awake();
    FindElements();
}

void WeaponView::Start()
{
    Component::Start();
    UmWatcher.Watch<WeaponViewModel, WeaponUIData>("Weapon", [this](const WeaponUIData& value) {
        if (value.Enable)
        {
            //gameObject->ActiveSelf = true;
            if (nullptr != _background)
                _background->SetImage(value.Background);
            if (nullptr != _weaponName)
                _weaponName->Text = value.WeaponName;
            if (nullptr != _weaponImage)
                _weaponImage->SetImage(value.WeaponIcon);
            if (nullptr != _hitDamage)
                _hitDamage->Text = std::to_string(value.HitDamage);
            if (nullptr != _criticalDamage)
                _criticalDamage->Text = std::to_string(value.CriticalDamage);
            if (nullptr != _speed)
                _speed->Text = std::to_string(value.Speed);
            if (nullptr != _attackCount)
                _attackCount->Text = std::to_string(value.AttackCount);
            if (nullptr != _description1)
                _description1->Description = WStringToU8(value.Description1);
            if (nullptr != _description2)
                _description2->Description = WStringToU8(value.Description2);
        }
        else
        {
            //gameObject->ActiveSelf = false;
        }
    });
    //gameObject->ActiveSelf = false;
}

void WeaponView::FindElements()
{
    const GameObject& owner          = gameObject;
    Transform&        ownerTransform = owner.transform;
    Transform::ForeachBFS(ownerTransform, [this](const Transform* transform) {
        GameObject& object = transform->gameObject;
        if (nullptr == _background)
            _background = FindImageElement("Weapon Background", object);
        if (nullptr == _weaponName)
            _weaponName = FindTextElement("Weapon Name", object);
        if (nullptr == _weaponImage)
            _weaponImage = FindImageElement("Weapon Image", object);
        if (nullptr == _hitDamage)
            _hitDamage = FindTextElement("Hit Damage", object);
        if (nullptr == _criticalDamage)
            _criticalDamage = FindTextElement("Critical Damage", object);
        if (nullptr == _speed)
            _speed = FindTextElement("Speed", object);
        if (nullptr == _attackCount)
            _attackCount = FindTextElement("Attack Count", object);
        if (nullptr == _description1)
            _description1 = FindDescriptionPanel("Description1", object);
        if (nullptr == _description2)
            _description2 = FindDescriptionPanel("Description2", object);
    });

    if (nullptr == _background)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Background Tag의 Image Element를 찾을 수 없습니다.");
    if (nullptr == _weaponName)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Weapon Name Tag의 Text Element를 찾을 수 없습니다.");
    if (nullptr == _weaponImage)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Weapon Image Tag의 Image Element를 찾을 수 없습니다.");
    if (nullptr == _hitDamage)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Hit Damage Tag의 Text Element를 찾을 수 없습니다.");
    if (nullptr == _criticalDamage)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Critical Damage Tag의 Text Element를 찾을 수 없습니다.");
    if (nullptr == _speed)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Speed Tag의 Text Element를 찾을 수 없습니다.");
    if (nullptr == _attackCount)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Attack Count Tag의 Text Element를 찾을 수 없습니다.");
    if (nullptr == _description1)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Description 1 Tag의 Description Panel을 찾을 수 없습니다.");
    if (nullptr == _description2)
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Description 2 Tag의 Description Panel을 찾을 수 없습니다.");
}

ImageElement* WeaponView::FindImageElement(const std::string& tag, GameObject& object)
{
    ImageElement* element = nullptr;
    if (object.CompareTag(tag))
    {
        element = object.GetComponent<ImageElement>();
    }
    return element;
}

TextElement* WeaponView::FindTextElement(const std::string& tag, GameObject& object)
{
    TextElement* element = nullptr;
    if (object.CompareTag(tag))
    {
        element = object.GetComponent<TextElement>();
    }
    return element;
}

DescriptionPanel* WeaponView::FindDescriptionPanel(const std::string& tag, GameObject& object)
{
    DescriptionPanel* element = nullptr;
    if (object.CompareTag(tag))
    {
        element = object.GetComponent<DescriptionPanel>();
    }
    return element;
}