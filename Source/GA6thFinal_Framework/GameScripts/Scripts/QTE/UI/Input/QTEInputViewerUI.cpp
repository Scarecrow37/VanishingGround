#include "pchScripts.h"
#include "QTEInputViewerUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Panels/Horizontal/HorizontalPanel.h"

namespace QTE
{
    void InputViewerUI::Initialize(size_t poolSize) 
    {

    }
    void InputViewerUI::MatchUIFromObject(GameObject& object)
    {
        Overlay = object.CompareTag(OVERLAY_TAG) 
            ? object.GetComponent<OverlayPanel>() 
            : Overlay;

        Horizontal = object.CompareTag(HORIZONTAL_TAG) 
            ? object.GetComponent<HorizontalPanel>() 
            : Horizontal;
    }
    void InputViewerUI::Active(bool active)
    {
        if (Overlay)
        {
            Overlay->gameObject->ActiveSelf = active;
        }
    }
    void InputViewerUI::Alpha(float alpha)
    {
        for (auto& inputNode : NodePool)
        {
            inputNode.Alpha(alpha);
        }
    }
    void InputViewerUI::Reset()
    {
        for (auto& inputNode : NodePool)
        {
            inputNode.Reset();
            Transform* parent = Overlay ? &Overlay->transform : nullptr;
            inputNode.SetParent(parent);
        }
        ActivedNode = 0;
    }
    void InputViewerUI::OnNotePressed(const QTE::NoteResult& result)
    {
        File::Guid&  guid = ButtonGuid[result.PressedButton];
        if (ActivedNode < NodePool.size())
        {
            InputNodeUI& node = NodePool[ActivedNode];
            node.SetImage(guid);
        }
    }
}