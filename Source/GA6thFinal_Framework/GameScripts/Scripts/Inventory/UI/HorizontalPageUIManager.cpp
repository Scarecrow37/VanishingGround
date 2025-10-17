#include "pchScripts.h"
#include "HorizontalPageUIManager.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Panels/Horizontal/HorizontalPanel.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "PlayerSystem/PlayerSystem.h"
#include "Inventory/UINavi/InventoryArrowNavi.h"

UMREAL_COMPONENT(HorizontalPageUIManager)

HorizontalPageUIManager::HorizontalPageUIManager() = default;
HorizontalPageUIManager::~HorizontalPageUIManager() = default;

void HorizontalPageUIManager::ImGuiDrawPropertysEvent() 
{
    if (UmCore->IsPlay())
    {
        if (_horizontalPanel)
        {  
            static int InstanceCount = 0;
            ImGui::InputInt("Instance", &InstanceCount);
            ImGui::SameLine();
            if (ImGui::Button("Create"))
            {
                UpdateHorizontalUI(InstanceCount);
            } 
            int focus = (int)_currentFocus;
            if (ImGui::InputInt("Focus Index", &focus))
            {
                focus = std::clamp(focus, 0, (int)_horizontalBar.size());
                SetHorizontalFocus(focus);
            }
        }
         
    }
}

void HorizontalPageUIManager::Awake()
{
    Base::Awake();
    FindUIElements();
}

void HorizontalPageUIManager::FindUIElements() 
{
    //화살표 찾기
    if (Transform* arrowPanel = transform->FindWithTag("Arrow Panel"))
    {
        Transform::ForeachBFS(*arrowPanel, [this](Transform* curr) 
        {
            GameObject& object = curr->gameObject;
            if (object.CompareTag("Left Arrow"))
            {
                _arrow.Left = object.GetComponent<ImageElement>();
                if (_arrow.LeftNavi = object.GetComponent<InventoryArrowNavi>())
                {
                    _arrow.LeftNavi->SetScrollManager(this);
                }
            }
            if (object.CompareTag("Right Arrow"))
            {
                _arrow.Right = object.GetComponent<ImageElement>();
                if (_arrow.RightNavi = object.GetComponent<InventoryArrowNavi>())
                {
                    _arrow.RightNavi->SetScrollManager(this);
                }
            }
        });
    }

    //Horizontal Panel 찾기
    if (Transform* horizontalPanel = transform->FindWithTag("Horizontal Bar"))
    {
        _horizontalPanel = horizontalPanel->gameObject->GetComponent<HorizontalPanel>();
    }
}

HorizontalPageUIManager::HorizontalBarElement HorizontalPageUIManager::CreateHorizontalElement()
{
    HorizontalBarElement element;
    if (_horizontalPanel)
    {
        auto panelobject = NewGameObject<GameObject>("Bar Panel");
        if (panelobject)
        {
            OverlayPanel& overlayPanel      = panelobject->AddComponent<OverlayPanel>();
            overlayPanel.Size               = {35, 21};
            overlayPanel.HorizontalAlign    = HorizontalAlignment::CENTER;
            overlayPanel.VerticalAlign      = VerticalAlignment::CENTER;
            panelobject->transform->SetParent(_horizontalPanel->transform);
       
            if (auto frameObject = NewGameObject<GameObject>("Slot Frame Element"))
            {
                ImageElement& image = frameObject->AddComponent<ImageElement>();
                image.SetImage(BAR_FRAME_GUID);
                image.Size = {25, 6};
                frameObject->transform->SetParent(panelobject->transform);
                element.Frame = &image;
            }

            if (auto focusObject = NewGameObject<GameObject>("Slot Focus Element"))
            {
                ImageElement& image = focusObject->AddComponent<ImageElement>();
                image.SetImage(BAR_FOCUS_GUID);
                image.Size = {33, 14};
                image.Enable = false;
                focusObject->transform->SetParent(panelobject->transform);
                element.Focus = &image;
            }
        }   
    }
    return element;
}

void HorizontalPageUIManager::UpdateArrow(size_t index) 
{
    size_t horizontalSize = _horizontalBar.size(); 
    if (horizontalSize > 1)
    {
        size_t lastIndex = horizontalSize - 1;
        if (index == 0)
        {
            _arrow.SetEnableLeft(false);
            _arrow.SetEnableRight(true);
        }
        else if (index == lastIndex)
        {
            _arrow.SetEnableLeft(true);
            _arrow.SetEnableRight(false);
        }
        else
        {
            _arrow.SetEnableLeft(true);
            _arrow.SetEnableRight(true);
        }
    }
    else
    {
        _arrow.SetEnableLeft(false);
        _arrow.SetEnableRight(false);
    }
}

void HorizontalPageUIManager::UpdateHorizontalUI(size_t count)
{
    for (auto& bar : _horizontalBar)
    {
        if (bar.Frame)
        {
            Transform* parent = bar.Frame->transform->Parent;
            if (parent)
            {
                GameObject::Destroy(parent->gameObject);
            }
        }
        if (bar.Focus)
        {
            Transform* parent = bar.Focus->transform->Parent;
            if (parent)
            {
                GameObject::Destroy(parent->gameObject);
            }
        }      
    }
    _horizontalBar.clear();

    if (count != 0)
    {
        FindUIElements();
        for (size_t i = 0; i < count; i++)
        {
            HorizontalBarElement element = CreateHorizontalElement();
            _horizontalBar.push_back(element);
        }        
    }
    SetHorizontalFocus(0);
}

void HorizontalPageUIManager::SetHorizontalFocus(size_t index) 
{
    if (index < _horizontalBar.size())
    {
        for (size_t i = 0; i < _horizontalBar.size(); i++)
        {
            auto& bar = _horizontalBar[i];
            if (i == index)
            {
                if (bar.Focus)
                {
                    bar.Focus->Enable = true;
                }
            }
            else
            {
                if (bar.Focus)
                {
                    bar.Focus->Enable = false;
                }
            }
        }
        if (_currentFocus == index)
        {
            _lastDIR = DIR::UNKNOWN;
        }
        else
        {
            if (_currentFocus < index)
            {
                _lastDIR = DIR::RIGHT;
            }
            else
            {
                _lastDIR = DIR::LEFT;
            }
        }
        _currentFocus = index;
    }
    UpdateArrow(index);
}

void HorizontalPageUIManager::HorizontalScrollLeft() 
{
    if (0 < _currentFocus)
    {
        SetHorizontalFocus(_currentFocus - 1);
    }
}

void HorizontalPageUIManager::HorizontalScrollRight() 
{
    if (false == _horizontalBar.empty())
    {
        const size_t maxIndex = _horizontalBar.size() - 1;
        if (_currentFocus < maxIndex)
        {
            SetHorizontalFocus(_currentFocus + 1);
        }
    }
}

void HorizontalPageUIManager::ArrowElement::SetEnableLeft(bool enable) 
{
    if (Left)
    {
        Left->Enable = enable;
    }
    if (LeftNavi)
    {
        LeftNavi->Enable = enable;
    }
}

void HorizontalPageUIManager::ArrowElement::SetEnableRight(bool enable) 
{
    if (Right)
    {
        Right->Enable = enable;
    }
    if (RightNavi)
    {
        RightNavi->Enable = enable;
    }
}
