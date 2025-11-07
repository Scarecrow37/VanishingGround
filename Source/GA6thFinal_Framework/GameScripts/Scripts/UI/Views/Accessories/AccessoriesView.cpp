#include "pchScripts.h"
#include "AccessoriesView.h"
#include "UI/Panels/Grid/GridPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Utility/SingletonHelper.h"
#include "AccessorySystem/AccessorySystem.h"
#include "KeyCallbackUINavi/KeyCallbackUINavi.h"
#include "UI/Panels/Description/DescriptionPanel.h"
#include "TooltipSystem/TooltipSystem.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"

UMREAL_COMPONENT(AccessoriesView)

AccessoriesView::AccessoriesView()
{
    _accessoriesGridPanel = nullptr;
}
AccessoriesView::~AccessoriesView() = default;

void AccessoriesView::Reset() 
{

}

void AccessoriesView::Awake() 
{
    FindChildElements();
}

void AccessoriesView::Start() 
{
    Watch();
    AddCallback();
}

void AccessoriesView::OnDestroy() 
{
    Blind();
    ClearCallback();
}

void AccessoriesView::FindChildElements()
{
    for (int i = 0; i < transform->ChildCount; ++i)
    {
        if (Transform* curr = transform->GetChild(i))
        {
            GameObject& currObject = curr->gameObject;
            if (nullptr == _accessoriesGridPanel)
            {
                if (currObject.CompareTag("Accessories Grid Panel"))
                {
                    _accessoriesGridPanel = currObject.GetComponent<GridPanel>();
                }
            }
            else
            {
                break;
            }
        }
    }

    if (_accessoriesGridPanel)
    {
        size_t frameIndex = 0;
        size_t iconIndex = 0;
        size_t focusIndex = 0;
        Transform::ForeachDFS(_accessoriesGridPanel->transform, [&](Transform* curr) 
        {
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Frame"))
            {
                if (_uiElements.size() <= frameIndex)
                {
                    _uiElements.resize(frameIndex + 1);
                }
                _uiElements[frameIndex].Frame = object.GetComponent<ImageElement>();
                ++frameIndex;
            }
            else if (object.CompareTag("Icon"))
            {
                if (_uiElements.size() <= frameIndex)
                {
                    _uiElements.resize(frameIndex + 1);
                }
                _uiElements[iconIndex].Icon = object.GetComponent<ImageElement>();
                ++iconIndex;
            }
            else if (object.CompareTag("Focus"))
            {
                if (_uiElements.size() <= focusIndex)
                {
                    _uiElements.resize(focusIndex + 1);
                }
                _uiElements[focusIndex].Focus = object.GetComponent<ImageElement>();
                ++focusIndex;
            }
        });
    }
}

void AccessoriesView::Watch() 
{
    if (_accessoriesGridPanel)
    {
        //기본 초기화
        for (size_t i = 0; i < _uiElements.size(); ++i)
        {
            if (_uiElements[i].Icon)
            {
                _uiElements[i].Icon->Enable = false;
            }              
            if (_uiElements[i].Frame)
            {
                _uiElements[i].Frame->Enable = false;
            } 
        }

        _handle = UmWatcher.Watch<AccessoriesViewModel, std::vector<AccessoriesUIData>>(
        VIEW_KEY, [this](const std::vector<AccessoriesUIData>& value) 
        {
            size_t playerAccessoriesCount = value.size();
            for (size_t i = 0; i <_uiElements.size(); ++i)
            {
                if (i < playerAccessoriesCount)
                {
                    const AccessoriesUIData& uiData = value[i];
                    if (_uiElements[i].Icon)
                    {
                        _uiElements[i].Icon->Enable = true;
                        _uiElements[i].Icon->SetImage(uiData.Icon);
                    }   

                    if (_uiElements[i].Frame)
                    {
                        _uiElements[i].Frame->Enable = true;
                    } 
                }
                else
                {
                    if (_uiElements[i].Icon)
                    {
                        _uiElements[i].Icon->Enable = false;
                    } 

                    if (_uiElements[i].Frame)
                    {
                        _uiElements[i].Frame->Enable = false;
                    } 
                }
            }

            if (auto naviPanel = GameObject::FindWithTag("Accessories Horizontal Navis Panel").lock())
            {
                for (int i = 0; i < naviPanel->transform->ChildCount; i++)
                {
                    if (Transform* child = naviPanel->transform->GetChild(i))
                    {
                        if (KeyCallbackUINavi* navi = child->gameObject->GetComponent<KeyCallbackUINavi>())
                        {
                            if (i < playerAccessoriesCount)
                            {
                                navi->Enable = true;
                            }
                            else
                            {
                                navi->Enable = false;
                            }
                        }
                    }
                }
            }
        });       

        if (AccessorySystem* system = SingletonComponent<AccessorySystem>::GetInstance())
        {
            system->NotifyUIModel();
        }

        gameObject->SetActive(false);
    }
}

void AccessoriesView::Blind() 
{
    if (_accessoriesGridPanel)
    {
        UmWatcher.Blind<AccessoriesViewModel>(VIEW_KEY, _handle);
    }
}

void AccessoriesView::AddCallback()
{
    std::string key;
    for (size_t i = 0; i < 15; i++)
    {
        key = "Accessories Panel Navi ";
        key += std::to_string(i);
        _callbacks.push_back(KeyCallbackUINavi::AddCallbackFocusIn(key, [this, i]() { FocusIn(i); }));
        _callbacks.push_back(KeyCallbackUINavi::AddCallbackFocusOut(key, [this, i]() { FocusOut(i); }));
        _callbacks.push_back(KeyCallbackUINavi::AddCallbackShowTooltips(key, [this, i]() { ShowTooltip(i); }));
        _callbacks.push_back(KeyCallbackUINavi::AddCallbackHideTooltips(key, [this, i]() { HideTooltip(i); }));
    }
    _focusInfoUIFade      = GameObject::FindComponentWithTag<FadeUIComponent>("Focus Info UI Panel");
    if (auto fade =_focusInfoUIFade.lock())
    {
        fade->Begin();
    }
    _focusInfoDescription = GameObject::FindComponentWithTag<DescriptionPanel>("Focus Info UI Description Panel");
}

void AccessoriesView::ClearCallback()
{
    for (auto& [delegate, handel] : _callbacks)
    {
        delegate->RemoveListener(handel);
    }
}

void AccessoriesView::FocusIn(size_t index)
{
    if (index < _uiElements.size())
    {
        if (_uiElements[index].Focus)
        {
            _uiElements[index].Focus->Enable = true;       
        }
    }
    if (TooltipSystem* tooltipSystem = SingletonComponent<TooltipSystem>::GetInstance())
    {
        tooltipSystem->Hide();
    }
}

void AccessoriesView::FocusOut(size_t index)
{
    if (index < _uiElements.size())
    {
        if (_uiElements[index].Focus)
        {
            _uiElements[index].Focus->Enable = false;
        }
    }
    if (TooltipSystem* tooltipSystem = SingletonComponent<TooltipSystem>::GetInstance())
    {
        tooltipSystem->Hide();
    }
    if (auto fade = _focusInfoUIFade.lock())
    {
        fade->FadeOut();
    }
}

void AccessoriesView::ShowTooltip(size_t index)
{
    if (auto fade = _focusInfoUIFade.lock())
    {
        fade->FadeIn();
    }

    if (AccessorySystem* accessorySystem = SingletonComponent<AccessorySystem>::GetInstance())
    {
        if (TooltipSystem* tooltipSystem = SingletonComponent<TooltipSystem>::GetInstance())
        {
            auto& accessories = accessorySystem->GetPlayerAccessoryItems();
            if (index < accessories.size())
            {
                DropItemInfo info = accessories[index].GetItemInfo();
                if (auto focusInfoDescription = _focusInfoDescription.lock())
                {
                    std::string description = DropItemInfo::GetArtifactDescription(info);
                    focusInfoDescription->Description = description;
                }
                std::vector<int> tooltips = DropItemInfo::GetArtifactTooltipIDs(info);
                tooltipSystem->Show(Tooltip::Group::PLAYER, tooltips);
            }          
        }
    }
}

void AccessoriesView::HideTooltip(size_t index)
{
    if (auto fade = _focusInfoUIFade.lock())
    {
        fade->FadeOut();
    }

    if (TooltipSystem* tooltipSystem = SingletonComponent<TooltipSystem>::GetInstance())
    {
        tooltipSystem->Hide();
    }
}