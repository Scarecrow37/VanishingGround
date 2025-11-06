#include "pchScripts.h"
#include "BattleGuideUI.h"
#include "UI/Elements/HoldingProgressImage/HoldingProgressImageElement.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Elements/Text/TextElement.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"

void BattleGuideUI::MatchUIFromObject(GameObject& object)
{
    if (object.CompareTag("Input Guide Panel"))
    {
        Overlay = object.GetComponent<OverlayPanel>();
        Fader   = object.GetComponent<FadeUIComponent>();
    }
    else if (object.CompareTag("A Guide"))
    {
        Transform::ForeachBFS(object.transform, [this](Transform* curr) {
            if (curr)
            {
                GameObject& child = curr->gameObject;
                if (child.CompareTag("Progress"))
                {
                    Progress = child.GetComponent<HoldingProgressImageElement>();
                }
                else if (child.CompareTag("Image"))
                {
                    ImageList[A] = child.GetComponent<ImageElement>();
                }
                else if (child.CompareTag("Text"))
                {
                    TextList[A] = child.GetComponent<TextElement>();
                }
            }
        });
    }
    else if (object.CompareTag("RB Guide"))
    {
        Transform::ForeachBFS(object.transform, [this](Transform* curr) {
            if (curr)
            {
                GameObject& child = curr->gameObject;
                if (child.CompareTag("Image"))
                {
                    ImageList[RB] = child.GetComponent<ImageElement>();
                }
                else if (child.CompareTag("Text"))
                {
                    TextList[RB] = child.GetComponent<TextElement>();
                }
            }
        });
    }
    else if (object.CompareTag("RT Guide"))
    {
        Transform::ForeachBFS(object.transform, [this](Transform* curr) {
            if (curr)
            {
                GameObject& child = curr->gameObject;
                if (child.CompareTag("Image"))
                {
                    ImageList[RT] = child.GetComponent<ImageElement>();
                }
                else if (child.CompareTag("Text"))
                {
                    TextList[RT] = child.GetComponent<TextElement>();
                }
            }
        });
    }
    else if (object.CompareTag("Start Guide"))
    {
        Transform::ForeachBFS(object.transform, [this](Transform* curr) {
            if (curr)
            {
                GameObject& child = curr->gameObject;
                if (child.CompareTag("Image"))
                {
                    ImageList[START] = child.GetComponent<ImageElement>();
                }
                else if (child.CompareTag("Text"))
                {
                    TextList[START] = child.GetComponent<TextElement>();
                }
            }
        });
    }
}
void BattleGuideUI::Active(bool active)
{
    if (Overlay)
    {
        Overlay->gameObject->ActiveSelf = active;
    }
}
void BattleGuideUI::Alpha(float alpha)
{
    for (auto& image : ImageList)
    {
        if (image)
        {
            image->Alpha = alpha;
        }
    }
    for (auto& text : TextList)
    {
        if (text)
        {
            {
                Color old = text->Color;
                old.w = alpha;
                text->Color = old;
            }
            {
                Color old = text->OutlineColor;
                old.w = alpha;
                text->OutlineColor = old;
            }
        }
    }
}
void BattleGuideUI::FadeIn()
{
    if (Fader)
    {
        Fader->FadeIn();
    }
}
void BattleGuideUI::FadeOut() 
{
    if (Fader)
    {
        Fader->FadeOut();
    }
}