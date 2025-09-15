#include "pchScripts.h"
#include "RevelationsView.h"

#include "Scripts/UI/Elements/Image/ImageElement.h"
#include "Scripts/UI/Elements/Text/TextElement.h"
#include "Scripts/UI/Panels/Description/DescriptionPanel.h"
#include "ViewModels/Revelations/RevelationsViewModel.h"

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
        "Revelations", [this](const std::vector<RevelationUIData>& revelations) {
            for (size_t i = 0; i < _revelationUis.size(); ++i)
            {
                if (i < revelations.size())
                {
                    if (nullptr != _revelationUis[i].IconElement)
                        _revelationUis[i].IconElement->SetImage(revelations[i].Icon);
                    if (nullptr != _revelationUis[i].NameElement)
                        _revelationUis[i].NameElement->Text = revelations[i].Name;
                    if (nullptr != _revelationUis[i].Description1Element)
                        _revelationUis[i].Description1Element->Description = WStringToU8(revelations[i].Description1);
                    if (nullptr != _revelationUis[i].Description2Element)
                        _revelationUis[i].Description2Element->Description = WStringToU8(revelations[i].Description2);
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
            .IconElement = nullptr, .NameElement = nullptr, .Description1Element = nullptr, .Description2Element = nullptr};

    Transform& ownerTransform = transform;

    Transform* revelationTransform = nullptr;
    Transform::ForeachBFS(ownerTransform, [&tag, &revelationObject, &revelationTransform](Transform* transform) {
        if (revelationTransform)
            return;
        GameObject& object = transform->gameObject;
        if (object.CompareTag(tag))
        {
            revelationObject    = &object;
            revelationTransform = transform;
        }
    });

    if (nullptr != revelationTransform)
    {
        Transform::ForeachBFS(*revelationTransform, [&revelationUI](const Transform* transform) {
            GameObject& object = transform->gameObject;
            if (nullptr == revelationUI.IconElement && object.CompareTag("Icon"))
            {
                revelationUI.IconElement = object.GetComponent<ImageElement>();
            }
            if (nullptr == revelationUI.NameElement && object.CompareTag("Name"))
            {
                revelationUI.NameElement = object.GetComponent<TextElement>();
            }
            if (nullptr == revelationUI.Description1Element && object.CompareTag("Description1"))
            {
                revelationUI.Description1Element = object.GetComponent<DescriptionPanel>();
            }
            if (nullptr == revelationUI.Description2Element && object.CompareTag("Description2"))
            {
                revelationUI.Description2Element = object.GetComponent<DescriptionPanel>();
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
        if (nullptr == revelationUI.Description1Element)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Description1 Element with tag '" + tag + "' not found.");
        }
        if (nullptr == revelationUI.Description2Element)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, "Description2 Element with tag '" + tag + "' not found.");
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Revelation UI with tag '" + tag + "' not found.");
    }

    return {revelationObject, revelationUI};
}