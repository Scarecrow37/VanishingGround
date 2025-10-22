#include "pchScripts.h"
#include "OpenInventoryComponent.h"
#include "Inventory/UI/InventoryUIManager.h"

UMREAL_COMPONENT(OpenInventoryComponent)

OpenInventoryComponent::OpenInventoryComponent()
{
    _openFlag = false;
}
OpenInventoryComponent::~OpenInventoryComponent() = default;

void OpenInventoryComponent::Awake() 
{
    if (ReflectFields->OpenButton != ControllerButton::UNKNOWN)
    {
        BindInputAction(ReflectFields->OpenButton, Action::PRESSED, this, &OpenInventoryComponent::OnButtonOpen);
    }
}

void OpenInventoryComponent::Update() 
{
    if (_openFlag)
    {
        if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
        {
            UINavigationComponent* focusNavi = nullptr;
            if (auto uiRootObject =  GameObject::Find("UI Root").lock())
            {
                if (UIRoot* root = uiRootObject->GetComponent<UIRoot>())
                {
                    focusNavi = root->GetFocusedNavigationComponent();
                }
            }
            manager->OpenInventory(focusNavi);
        }
        _openFlag = false;
    }
}

void OpenInventoryComponent::OnButtonOpen(const Input::Controller&)
{
    if (EnableInHierarchy)
    {
        _openFlag = true;
    }
}
