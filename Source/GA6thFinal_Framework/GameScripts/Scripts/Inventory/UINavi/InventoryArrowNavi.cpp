#include "pchScripts.h"
#include "InventoryArrowNavi.h"
#include "Inventory/UI/HorizontalPageUIManager.h"
#include "Inventory/UI/InventoryUIManager.h"

UMREAL_COMPONENT(InventoryArrowNavi)

InventoryArrowNavi::InventoryArrowNavi() = default;
InventoryArrowNavi::~InventoryArrowNavi() = default;

void InventoryArrowNavi::SetScrollManager(HorizontalPageUIManager* manager) 
{
    if (manager)
    {
        _scrollManager = manager->GetWeakPtr();
    }
}

void InventoryArrowNavi::FocusIn(FocusCallType callType) 
{
    Base::FocusIn(callType);
    if (auto manager = _scrollManager.lock())
    {
        if (HorizontalPageUIManager* panelManager = static_cast<HorizontalPageUIManager*>(manager.get()))
        {
            ArrowDir dir = ReflectFields->DIR;
            switch (dir)
            {
            case InventoryArrowNavi::ArrowDir::LEFT:
                panelManager->HorizontalScrollLeft();
                break;
            case InventoryArrowNavi::ArrowDir::RIGHT:
                panelManager->HorizontalScrollRight();
                break;
            default:
                break;
            }
            if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
            {
                manager->UpdateScroll(panelManager);
            }
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"HorizontalPageUIManager를 설정하지 않았습니다.");
    }
}
