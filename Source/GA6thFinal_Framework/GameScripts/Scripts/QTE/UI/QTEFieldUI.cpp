#include "pchScripts.h"
#include "QTEFieldUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

namespace QTE
{
    void FieldUI::MatchUIFromObject(GameObject& object) 
    {
        Overlay = object.CompareTag(OVERLAY_TAG) 
            ? object.GetComponent<OverlayPanel>() 
            : Overlay;

        Line = object.CompareTag(LINE_TAG) 
            ? object.GetComponent<ImageElement>() 
            : Line;

        Flow = object.CompareTag(FLOW_TAG) 
            ? object.GetComponent<SpriteAnimationElement>() 
            : Flow;

        JudgeNote = object.CompareTag(JUDGE_TAG) 
            ? object.GetComponent<SpriteAnimationElement>() 
            : JudgeNote;

        StartAnimation = object.CompareTag(START_ANIMATION_TAG) 
            ? object.GetComponent<SpriteAnimationElement>() 
            : StartAnimation;
    }
} // namespace QTE