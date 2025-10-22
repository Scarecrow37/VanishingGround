#include "pchScripts.h"
#include "WeaponView.h"

#include "ViewModels/Weapon/WeaponViewModel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"

UMREAL_COMPONENT(WeaponView)

WeaponView::WeaponView() 
    : 
    _singletonComponent(this)
{
    _descriptionUI  = nullptr;
    _iconUI         = nullptr;
    _nameUI         = nullptr;
}

WeaponView::~WeaponView()
{
    
}

void WeaponView::Focus(bool value) 
{
    if (value)
    {
        if (_backgroundUI.ImageOn)
            _backgroundUI.ImageOn->Enable = true;

        if (_backgroundUI.ImageOff)
            _backgroundUI.ImageOff->Enable = false;
    }
    else
    {
        if (_backgroundUI.ImageOn)
            _backgroundUI.ImageOn->Enable = false;

        if (_backgroundUI.ImageOff)
            _backgroundUI.ImageOff->Enable = true;
    }
}

void WeaponView::Awake()
{
    Component::Awake();
    _singletonComponent.TrySingleTon();
    FindElements();
}

void WeaponView::Start()
{
    Component::Start();
    _watchHandle = UmWatcher.Watch<WeaponViewModel, WeaponUIData>("Weapon", [this](const WeaponUIData& value) 
    {
        if (value.Enable)
        {
            if (false == gameObject->ActiveSelf)
            {
                gameObject->ActiveSelf = true;
            }

            if (_nameUI)
            {
                _nameUI->Text = value.WeaponName;
                _nameUI->Color = value.GradeColor;
            }
                
            if (_iconUI)
                _iconUI->SetImage(value.WeaponIcon);

            if (_textInfoUI.Damage)
                _textInfoUI.Damage->Text = value.HitDamage;

            if (_textInfoUI.Critical)
                _textInfoUI.Critical->Text = value.CriticalDamage;

            if (_textInfoUI.AttackCount)
                _textInfoUI.AttackCount->Text = value.AttackCount;

            if (_textInfoUI.Speed)
                _textInfoUI.Speed->Text = value.Speed;
            
            if (_descriptionUI)
                _descriptionUI->Description = value.Description;
        }
        else
        {
            gameObject->ActiveSelf = false;
        }
    });
    gameObject->ActiveSelf = false;
}

void WeaponView::OnDestroy() 
{
    UmWatcher.Blind<WeaponViewModel>("Weapon", _watchHandle);
}

void WeaponView::FindElements()
{
    FindBackgroundUI();
    FindTextInfoUI();
    FindDiscriptionUI();
    FindIconUI();
    FindNameUI();
}

void WeaponView::FindBackgroundUI() 
{
    _backgroundUI.BackGroundPanel = GameObject::FindWithTag("Background Panel").lock().get();
    if (_backgroundUI.BackGroundPanel)
    {
        Transform::ForeachDFS(_backgroundUI.BackGroundPanel->transform, [&](Transform* curr) 
        {   
            if (nullptr == _backgroundUI.ImageOff)
            {
                if (curr->gameObject->CompareTag("Weapon Focus off"))
                {
                    if (_backgroundUI.ImageOff = curr->gameObject->GetComponent<ImageElement>(); nullptr == _backgroundUI.ImageOff)
                    {
                        std::u8string message = u8"Weapon Focus off에 Image Panel이 존재하지 않습니다.";
                        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                    }
                }
            }
            if (nullptr == _backgroundUI.ImageOn)
            {
                if (curr->gameObject->CompareTag("Weapon Focus on"))
                {
                    _backgroundUI.ImageOn = curr->gameObject->GetComponent<ImageElement>();
                    if (_backgroundUI.ImageOn)
                    {
                        _backgroundUI.ImageOn->Enable = false;
                    }
                    else
                    {
                        std::u8string message = u8"Weapon Focus on에 Image Panel이 존재하지 않습니다.";
                        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                    }
                }
            }
        });
    }
    else
    {
        std::u8string message = u8"WeaponView 자식에 Background Panel이 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }
}

void WeaponView::FindTextInfoUI() 
{
    _textInfoUI.TextInfoPanel = GameObject::FindWithTag("Text Info Panel").lock().get();
    if (_textInfoUI.TextInfoPanel)
    {
        Transform::ForeachDFS(_textInfoUI.TextInfoPanel->transform, [&](Transform* curr) 
        {           
            if (curr->gameObject->CompareTag("Text Damage"))
            {
                _textInfoUI.Damage = curr->gameObject->GetComponent<TextElement>();
                if (nullptr == _textInfoUI.Damage)
                {
                    std::u8string message = u8"Text Damage에 Text Element이 존재하지 않습니다.";
                    UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                }
            }
            else if (curr->gameObject->CompareTag("Text Critical"))
            {
                _textInfoUI.Critical = curr->gameObject->GetComponent<TextElement>();
                if (nullptr == _textInfoUI.Critical)
                {
                    std::u8string message = u8"Text Critical에 Text Element이 존재하지 않습니다.";
                    UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                }
            }
            else if (curr->gameObject->CompareTag("Text Attack Count"))
            {
                _textInfoUI.AttackCount = curr->gameObject->GetComponent<TextElement>();
                if (nullptr == _textInfoUI.AttackCount)
                {
                    std::u8string message = u8"Text Attack Count에 Text Element이 존재하지 않습니다.";
                    UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                }
            }
            else if (curr->gameObject->CompareTag("Text Speed"))
            {
                _textInfoUI.Speed = curr->gameObject->GetComponent<TextElement>();
                if (nullptr == _textInfoUI.Speed)
                {
                    std::u8string message = u8"Text Speed에 Text Element이 존재하지 않습니다.";
                    UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                }
            }
        });
    }
    else
    {
        std::u8string message = u8"WeaponView 자식에 Text Info Panel이 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }

}

void WeaponView::FindDiscriptionUI() 
{
    if (std::vector<GameObject*> objects = transform->FindBFSwithTag("Weapon Description"); false == objects.empty())
    {
        _descriptionUI = objects.front()->GetComponent<DescriptionPanel>();
    }
    else
    {
        std::u8string message = u8"WeaponView 자식에 Weapon Description이 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }  
}

void WeaponView::FindIconUI() 
{
    if (std::vector<GameObject*> objects = transform->FindBFSwithTag("Weapon Icon"); false == objects.empty())
    {
        _iconUI = objects.front()->GetComponent<ImageElement>();
    }
    else
    {
        std::u8string message = u8"WeaponView 자식에 Weapon Icon이 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }
}

void WeaponView::FindNameUI() 
{
    if (std::vector<GameObject*> objects = transform->FindBFSwithTag("Weapon Name Text"); false == objects.empty())
    {
        _nameUI = objects.front()->GetComponent<TextElement>();
    }
    else
    {
        std::u8string message = u8"WeaponView 자식에 Weapon Name Text가 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }
}
