#include "pchScripts.h"
#include "WeaponView.h"

#include "ViewModels/Weapon/WeaponViewModel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"
#include "TutorialSystem/TutorialSystem.h"

UMREAL_COMPONENT(WeaponView)

WeaponView::WeaponView() 
    : 
    _singletonComponent(this)
{
    _descriptionUI     = nullptr;
    _iconUI            = nullptr;
    _nameUI            = nullptr;

    _rootFadeUI       = nullptr;
    _textFadeUI        = nullptr;
    _descriptionFadeUI = nullptr;
    _iconFadeUI        = nullptr;
    _nameFadeUI        = nullptr;
}

WeaponView::~WeaponView()
{
    
}

void WeaponView::Focus(bool value) 
{
    if (value)
    {
        if (_backgroundUI.FocusOn)
            _backgroundUI.FocusOn->Enable = true;

        if (_backgroundUI.FocusOff)
            _backgroundUI.FocusOff->Enable = false;
    }
    else
    {
        if (_backgroundUI.FocusOn)
            _backgroundUI.FocusOn->Enable = false;

        if (_backgroundUI.FocusOff)
            _backgroundUI.FocusOff->Enable = true;
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
            if (_backgroundUI.FocusOff)
            {
                float duration = _backgroundUI.FocusOff->Duration;

                float fadeInStartTime = duration * _backgroundUI.FocusOff->GetAnimationProgress(ReflectFields->FadeInFrame[0]);
                float fadeInEndTime   = duration * _backgroundUI.FocusOff->GetAnimationProgress(ReflectFields->FadeInFrame[1]);
                float fadeInTime      = fadeInEndTime - fadeInStartTime;
                
                float fadeOutStartTime = duration * _backgroundUI.FocusOff->GetAnimationProgress(ReflectFields->FadeOutFrame[0]);
                float fadeOutEndTime   = duration * _backgroundUI.FocusOff->GetAnimationProgress(ReflectFields->FadeOutFrame[1]);
                float fadeOutTime      = fadeOutEndTime - fadeOutStartTime;

                auto fadeCall = [](FadeUIComponent* ui, float duration, bool fadeIn = true) 
                {   
                    if (ui)
                    {
                        ui->FadeDuration = duration;
                        if (fadeIn)
                        {
                            ui->FadeIn();
                        }                         
                        else
                        {
                            ui->FadeOut();
                        }                           
                    }
                };

                auto updateUIInfo = [this, value]() 
                {
                    if (_nameUI)
                    {
                        _nameUI->Text  = value.WeaponName;
                        Color color    = value.GradeColor;
                        color.w        = _nameUI->Alpha;
                        _nameUI->Color = color;
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
                };

                if (false == gameObject->ActiveSelf)
                {
                    gameObject->ActiveSelf = true;
                    if (_rootFadeUI)
                    {
                        updateUIInfo();
                        _rootFadeUI->FadeIn();
                        UmTime.Invoke(_rootFadeUI->FadeDuration, []() 
                        {
                            if (TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance())
                            {
                                system->Show(805901);
                            }
                        });
                    }
                }
                else
                {
                    //무기 재등장 사운드
                    UmAudio.Play("-431002");

                    _backgroundUI.FocusOff->Setup();
                    _backgroundUI.FocusOff->StartAnimation();

                    UmTime.Invoke(this, fadeOutStartTime, [this, fadeCall, fadeOutTime]() 
                    {
                        fadeCall(_textFadeUI, fadeOutTime, false);
                        fadeCall(_descriptionFadeUI, fadeOutTime, false);
                        fadeCall(_iconFadeUI, fadeOutTime, false);
                        fadeCall(_nameFadeUI, fadeOutTime, false);
                    });

                    UmTime.Invoke(this, fadeInStartTime, [this, fadeCall, fadeInTime]() 
                    {
                        fadeCall(_textFadeUI, fadeInTime);
                        fadeCall(_descriptionFadeUI, fadeInTime);
                        fadeCall(_iconFadeUI, fadeInTime);
                        fadeCall(_nameFadeUI, fadeInTime);
                    });

                    UmTime.Invoke(this, fadeOutEndTime, updateUIInfo);
                }              
            }
        }
    });
    gameObject->ActiveSelf = false;
    if (_rootFadeUI)
    {
        _rootFadeUI->Begin();
    }
}

void WeaponView::OnDestroy() 
{
    UmWatcher.Blind<WeaponViewModel>("Weapon", _watchHandle);
}

void WeaponView::FindElements()
{    
    _rootFadeUI = GetComponent<FadeUIComponent>();
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
            if (nullptr == _backgroundUI.FocusOff)
            {
                if (curr->gameObject->CompareTag("Weapon Focus off"))
                {
                    if (_backgroundUI.FocusOff = curr->gameObject->GetComponent<SpriteAnimationElement>(); nullptr == _backgroundUI.FocusOff)
                    {
                        std::u8string message = u8"Weapon Focus off에 SpriteAnimationElement이 존재하지 않습니다.";
                        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
                    }
                }
            }
            if (nullptr == _backgroundUI.FocusOn)
            {
                if (curr->gameObject->CompareTag("Weapon Focus on"))
                {
                    _backgroundUI.FocusOn = curr->gameObject->GetComponent<ImageElement>();
                    if (_backgroundUI.FocusOn)
                    {
                        _backgroundUI.FocusOn->Enable = false;
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
        _textFadeUI = _textInfoUI.TextInfoPanel->GetComponent<FadeUIComponent>();
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
        _descriptionFadeUI = objects.front()->GetComponent<FadeUIComponent>();
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
        _iconFadeUI = objects.front()->GetComponent<FadeUIComponent>();
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
        _nameFadeUI = objects.front()->GetComponent<FadeUIComponent>();
    }
    else
    {
        std::u8string message = u8"WeaponView 자식에 Weapon Name Text가 존재하지 않습니다.";
        UmLogger.Log(LogLevel::LEVEL_WARNING, message);
    }
}
