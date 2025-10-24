#include "pchScripts.h"
#include "QTEGuideUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"

namespace QTE
{
    void GuideUI::MatchUIFromObject(GameObject& object) 
    {
        if (object.CompareTag(OVERLAY_TAG))
        {
            Overlay = object.GetComponent<OverlayPanel>();
        }
        else
        {
            for (int i = 0; i < BUTTON_COUNT; ++i)
            {
                if (object.CompareTag(GUIDE_BUTTON_TAG[i]))
                {
                    GuideButton[i] = object.GetComponent<ImageElement>();
                }
                if (object.CompareTag(GUIDE_TEXT_TAG[i]))
                {
                    GuideText[i] = object.GetComponent<TextElement>();
                }
            }
        }
    }
    void GuideUI::Active(bool active) 
    {
        if (Overlay)
        {
            Overlay->gameObject->ActiveSelf = active;
        }
    }
    void GuideUI::Alpha(float alpha) 
    {
        for (int i = 0; i < BUTTON_COUNT; ++i)
        {
            if (GuideButton[i])
            {
                GuideButton[i]->Alpha = alpha;
            }
            if (GuideText[i])
            {
                //GuideText[i].
            }
        }
    }
}
