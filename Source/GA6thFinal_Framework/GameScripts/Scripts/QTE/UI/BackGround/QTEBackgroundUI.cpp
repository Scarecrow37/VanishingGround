#include "pchScripts.h"
#include "QTEBackgroundUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

namespace QTE
{
    void BackgroundUI::MatchUIFromObject(GameObject& object) 
    {
        if (object.CompareTag(BACKGROUND_TAG))
        {
            Background = object.GetComponent<ImageElement>();
        }
    }
    void BackgroundUI::Active(bool active) 
    {
        if (Background)
        {
            Background->gameObject->ActiveSelf = active;
        }
    }
    void BackgroundUI::Alpha(float alpha) 
    {
        if (Background)
        {
            Background->Alpha = alpha;
        }
    }
    void BackgroundUI::Reset() 
    {
    }

}