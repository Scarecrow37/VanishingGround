#include "pchScripts.h"
#include "StageFocusView.h"
#include "Map/Stage.h"
#include "Map/SmoothScroll.h"
#include "Map/ViewModels/StageFocusViewModel.h"
#include "UI/Elements/Image/ImageElement.h"

StageFocusView::StageFocusView() = default;
StageFocusView::~StageFocusView() = default;

void StageFocusView::Watch(const std::string& key)
{
    if (false == key.empty())
    {
        UmWatcher.Blind<StageFocusViewModel>(key);
        try
        {
            UmWatcher.Watch<StageFocusViewModel, Stage*>(key, 
                [this](const Stage* value) 
                {                     
                    if (auto imageElement = value->GetComponent<ImageElement>(); imageElement)
                    {
                        ImageElement* focusElement = GetComponent<ImageElement>();
                        SIZE          focusSize    = focusElement->Size;
                        POINT         targetPoint  = imageElement->Point;
                        SIZE          size         = imageElement->Size;

                        POINT alignPoint = AlignPoint()(HorizontalAlignment::CENTER, VerticalAlignment::CENTER, focusSize - size);
                        POINT focusPoint = targetPoint - alignPoint;
                        focusElement->Point = focusPoint;                        

                        const SIZE& resolution = UmGraphics.GetResolution();
                        
                        LONG offset = resolution.cy >> 1;
                        if (focusPoint.y + offset > resolution.cy)
                        {
                            _scroll->SetScrollTarget((focusPoint.y + offset) / (float)_scrollSize.cy);
                        }
                        else
                        {
                            _scroll->SetScrollTarget(0.f);
                        }
                    }
                });

            if (auto scroll = GameObject::Find("Scroll").lock(); scroll)
            {
                _scroll = scroll->GetComponent<SmoothScroll>();
                
                if (auto background = GameObject::Find("Background").lock(); background)
                {
                    _scrollSize = background->GetComponent<ImageElement>()->Size;
                }
            }
        }
        catch (const std::exception& e)
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "Watch Failed.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "StageView: WatchKey is empty.");
    }
}
