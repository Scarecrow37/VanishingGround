#include "pchScripts.h"
#include "AccessoriesView.h"
#include "UI/Panels/Grid/GridPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "Utility/SingletonHelper.h"
#include "AccessorySystem/AccessorySystem.h"

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
}

void AccessoriesView::OnDestroy() 
{
    Blind();
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
        std::vector<GameObject*> frames = _accessoriesGridPanel->transform->FindDFSwithTag("Frame");
        for (auto& frame : frames)
        {
            auto& ui = _uiElements.emplace_back();
            ui.Frame = frame->GetComponent<ImageElement>();
        }

        std::vector<GameObject*> icons = _accessoriesGridPanel->transform->FindDFSwithTag("Icon");
        for (size_t i = 0; i < icons.size(); ++i)
        {
            if (i < _uiElements.size())
            {
                _uiElements[i].Icon = icons[i]->GetComponent<ImageElement>();
            }
        }
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
            for (size_t i = 0; i <_uiElements.size(); ++i)
            {
                if (i < value.size())
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
