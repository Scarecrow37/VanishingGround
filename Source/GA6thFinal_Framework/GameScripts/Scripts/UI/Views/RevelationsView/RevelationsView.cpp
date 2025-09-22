#include "pchScripts.h"
#include "RevelationsView.h"

#include "Scripts/UI/Elements/Image/ImageElement.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "Scripts/UI/Panels/Description/DescriptionPanel.h"
#include "ViewModels/Revelations/RevelationsViewModel.h"

UMREAL_COMPONENT(RevelationsView)

RevelationsView::~RevelationsView()
{
    UmWatcher.Blind<RevelationsViewModel>("Revelations", _watchHandle);
}

void RevelationsView::Awake()
{
    Component::Awake();

    FindRevelationUIs();
}

void RevelationsView::Start()
{
    Component::Start();

    _watchHandle = UmWatcher.Watch<RevelationsViewModel, std::vector<RevelationUIData>>(
        "Revelations", [this](const std::vector<RevelationUIData>& revelations) 
        {
            for (size_t i = 0; i < _revelationUis.size(); ++i)
            {
                if (i < revelations.size())
                {
                    if (nullptr != _revelationUis[i].IconElement)
                        _revelationUis[i].IconElement->SetImage(revelations[i].Icon);
                    if (nullptr != _revelationUis[i].NameElement)
                        _revelationUis[i].NameElement->Text = revelations[i].Name;
                    if (nullptr != _revelationUis[i].DescriptionElement)
                        _revelationUis[i].DescriptionElement->Description = revelations[i].Description;
                    if (nullptr != _revelationUis[i].GradeElement)
                        _revelationUis[i].GradeElement->SetImage(revelations[i].Grade);
                }
            }
        });
}

void RevelationsView::FindRevelationUIs()
{
    auto [firstRevelationObject, firstRevelationUI]   = FindRevelationUI("1st Revelation");
    _revelationObjects[0]                             = firstRevelationObject;
    _revelationUis[0]                                 = firstRevelationUI;
    auto [secondRevelationObject, secondRevelationUI] = FindRevelationUI("2nd Revelation");
    _revelationObjects[1]                             = secondRevelationObject;
    _revelationUis[1]                                 = secondRevelationUI;
    auto [thirdRevelationObject, thirdRevelationUI]   = FindRevelationUI("3rd Revelation");
    _revelationObjects[2]                             = thirdRevelationObject;
    _revelationUis[2]                                 = thirdRevelationUI;
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
            if (nullptr == revelationUI.GradeElement && object.CompareTag("Grade"))
            {
                revelationUI.GradeElement = object.GetComponent<ImageElement>();
            }
            if (nullptr == revelationUI.DescriptionElement && object.CompareTag("Description"))
            {
                revelationUI.DescriptionElement = object.GetComponent<DescriptionPanel>();
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
        if (nullptr == revelationUI.GradeElement)
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