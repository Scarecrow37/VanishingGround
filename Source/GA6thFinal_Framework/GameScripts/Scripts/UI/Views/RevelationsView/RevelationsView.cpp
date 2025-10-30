#include "pchScripts.h"
#include "RevelationsView.h"

#include "Scripts/UI/Elements/Image/ImageElement.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "Scripts/UI/Panels/Description/DescriptionPanel.h"
#include "ViewModels/Revelations/RevelationsViewModel.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include "TutorialSystem/TutorialSystem.h"
#include "UI/Animations/ChildsAnimationsController/ChildsAnimationsController.h"

UMREAL_COMPONENT(RevelationsView)

RevelationsView::~RevelationsView() = default;

void RevelationsView::Awake()
{
    gameObject->AddTag(TAG);
    Component::Awake();
    FindRevelationUIs();
}

void RevelationsView::Start()
{
    Component::Start();
    _watchHandle = UmWatcher.Watch<RevelationsViewModel, std::vector<RevelationUIData>>(
    "Revelations", [this, thisWeak = GetWeakPtr()](const std::vector<RevelationUIData>& revelations) 
    {
        if (false == thisWeak.expired())
        {
            auto UpdateUIInfo = [this, revelations]() 
            {
                for (size_t i = 0; i < _revelationUis.size(); ++i)
                {
                    if (i < revelations.size())
                    {
                        if (nullptr != _revelationUis[i].IconElement)
                            _revelationUis[i].IconElement->SetImage(revelations[i].Icon);
                        if (nullptr != _revelationUis[i].NameElement)
                        {
                            _revelationUis[i].NameElement->Text = revelations[i].Name;

                            Color color                          = revelations[i].NameColor;
                            color.w                              = _revelationUis[i].NameElement->Alpha;
                            _revelationUis[i].NameElement->Color = color;
                        }
                        if (nullptr != _revelationUis[i].DescriptionElement)
                            _revelationUis[i].DescriptionElement->Description = revelations[i].Description;
                        if (false != _revelationUis[i].GradeElements.size())
                        {
                            size_t index = static_cast<size_t>(revelations[i].Grade);
                            for (auto& ui : _revelationUis[i].GradeElements)
                            {
                                ui->Enable = false;
                            }
                            if (index < _revelationUis[i].GradeElements.size())
                            {
                                _revelationUis[i].GradeElements[index]->Enable = true;
                            }
                        }
                    }
                }
            };         

            if (false == gameObject->ActiveSelf)
            {
                gameObject->ActiveSelf = true;
                if (auto reloadAnimation = _reloadAnimation.lock())
                {
                    reloadAnimation->Enable = false;
                }

                if (auto startAnimation = _startAnimation.lock())
                {
                    startAnimation->Enable = true;
                    startAnimation->StartAnimation();
                    if (auto fade = _textsFade.lock())
                    {
                        float time = startAnimation->Duration;
                        UmTime.Invoke(fade.get(), time, [fadeText = fade.get()]() 
                        {
                            fadeText->FadeIn();
                        });
                    }               
                }

                UpdateUIInfo();
            }
            else
            {
                if (auto reloadAnimation = _reloadAnimation.lock())
                {
                    reloadAnimation->Enable = true;
                    if (auto fade = _textsFade.lock())
                    {
                        float aniTime  = reloadAnimation->Duration;
                        float fadeTime = fade->FadeDuration;
                        fade->FadeOut();
                        UmTime.Invoke(fade.get(), fadeTime, [fadeText = fade.get(), this, UpdateUIInfo]()
                        {
                            UpdateUIInfo();
                            if (auto reloadAnimation = _reloadAnimation.lock())
                            {
                                reloadAnimation->StartAnimation();
                            }
                        });

                        UmTime.Invoke(fade.get(), fadeTime + aniTime, [fadeText = fade.get()]() 
                        {   
                            fadeText->FadeIn();
                        });
                    }
                }

                if (auto startAnimation = _startAnimation.lock())
                {
                    startAnimation->Enable = false;
                }
            }

        }       
    });

    if (auto textsFade = _textsFade.lock())
    {
        textsFade->Begin();
    }
    if (auto reload = _reloadAnimation.lock())
    {
        reload->Enable = false;
    }
    if (auto start = _startAnimation.lock())
    {
        start->Enable = false;
    }
    gameObject->ActiveSelf = false;
}

void RevelationsView::OnDestroy() 
{
    UmWatcher.Blind<RevelationsViewModel>("Revelations", _watchHandle);
    ClearRevelationUIs();
}

void RevelationsView::FindRevelationUIs()
{
    ClearRevelationUIs();
    auto [firstRevelationObject, firstRevelationUI]   = FindRevelationUI("1st Revelation");
    _revelationObjects[0]                             = firstRevelationObject;
    _revelationUis[0]                                 = firstRevelationUI;
    auto [secondRevelationObject, secondRevelationUI] = FindRevelationUI("2nd Revelation");
    _revelationObjects[1]                             = secondRevelationObject;
    _revelationUis[1]                                 = secondRevelationUI;
    auto [thirdRevelationObject, thirdRevelationUI]   = FindRevelationUI("3rd Revelation");
    _revelationObjects[2]                             = thirdRevelationObject;
    _revelationUis[2]                                 = thirdRevelationUI;

    if (FadeUIComponent* fadeUI = GetComponent<FadeUIComponent>())
    {
        _rootFade = fadeUI->GetWeakPtrAs<FadeUIComponent>();
    }

    if (Transform* texts = transform->FindWithTag("Texts"))
    {
        if (FadeUIComponent* fadeUI = texts->gameObject->GetComponent<FadeUIComponent>())
        {
            _textsFade = fadeUI->GetWeakPtrAs<FadeUIComponent>();
        }
    }

    if (Transform* animations = transform->FindWithTag("Animations"))
    { 
        Transform::ForeachBFS(*animations, [this](Transform* curr) 
        {
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Start"))
            {
                if (SpriteAnimationElement* animation = object.GetComponent<SpriteAnimationElement>())
                {
                     _startAnimation = animation->GetWeakPtrAs<SpriteAnimationElement>();
                }             
            }
            else if (object.CompareTag("Reload"))
            {
                if (SpriteAnimationElement* animation = object.GetComponent<SpriteAnimationElement>())
                {
                    _reloadAnimation = animation->GetWeakPtrAs<SpriteAnimationElement>();
                }
            }
        });
    }
}

std::pair<GameObject*, RevelationUI> RevelationsView::FindRevelationUI(const std::string& tag) const
{
    GameObject*  revelationObject = nullptr;
    RevelationUI revelationUI     = {
        .IconElement = nullptr, 
        .NameElement = nullptr, 
        .DescriptionElement = nullptr,
    };

    Transform& ownerTransform = transform;
    Transform* revelationTransform = nullptr;

    if (auto findObject = GameObject::FindWithTag(tag).lock())
    {
        revelationObject = findObject.get();
        revelationTransform = &revelationObject->transform;
    }

    if (nullptr != revelationTransform)
    {
        Transform::ForeachBFS(*revelationTransform, [&revelationUI](const Transform* transform) 
        {
            GameObject& object = transform->gameObject;
            if (nullptr == revelationUI.IconElement && object.CompareTag("Icon"))
            {
                revelationUI.IconElement = object.GetComponent<ImageElement>();
            }
            if (nullptr == revelationUI.NameElement && object.CompareTag("Name"))
            {
                revelationUI.NameElement = object.GetComponent<TextElement>();
            }
            if (object.CompareTag("Grade"))
            {
                revelationUI.GradeElements.push_back(object.GetComponent<ImageElement>());
            }
            if (nullptr == revelationUI.DescriptionElement && object.CompareTag("Description"))
            {
                revelationUI.DescriptionElement = object.GetComponent<DescriptionPanel>();
            }
            if (nullptr == revelationUI.AnimationsController && object.CompareTag("Animations"))
            {
                revelationUI.AnimationsController = object.GetComponent<ChildsAnimationsController>();
            }
        });

        if (nullptr == revelationUI.IconElement)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Icon Element with tag '" + tag + "' not found.");
        }
        if (nullptr == revelationUI.NameElement)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Name Element with tag '" + tag + "' not found.");
        }
        if (true == revelationUI.GradeElements.empty())
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Grade Element with tag '" + tag + "' not found.");
        }
        if (nullptr == revelationUI.DescriptionElement)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Description1 Element with tag '" + tag + "' not found.");
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Revelation UI with tag '" + tag + "' not found.");
    }

    return {revelationObject, revelationUI};
}

void RevelationsView::ClearRevelationUIs() 
{
    for (auto& uis : _revelationUis)
    {
        uis.DescriptionElement = nullptr;
        uis.IconElement        = nullptr;
        uis.NameElement        = nullptr;
        uis.GradeElements.clear();
    }

    for (auto& objs : _revelationObjects)
    {
        objs = nullptr;
    }
}
