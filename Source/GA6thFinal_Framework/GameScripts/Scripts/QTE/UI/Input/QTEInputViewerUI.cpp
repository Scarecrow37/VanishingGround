#include "pchScripts.h"
#include "QTEInputViewerUI.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Panels/Horizontal/HorizontalPanel.h"

namespace QTE
{
    void InputViewerUI::Initialize(File::Guid prefabGuid, size_t poolSize) 
    {
        NodePool.clear();
        if (Overlay)
        {
            Transform& parent = Overlay->transform;
            for (int i = 0; i < poolSize; ++i)
            {
                NodePool.emplace_back(prefabGuid, &parent);
            }
        }
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
        if (Overlay)
        {
            for (auto& inputNode : NodePool)
            {
                inputNode.Reset();
                Transform& parent = Overlay->transform;
                inputNode.SetParent(&parent);
            }
        }
        ActivedNode = 0;
    }
    void InputViewerUI::OnNotePressed(const QTE::NoteResult& result)
    {
        if (result.IsPressedButton())
        {
            File::Guid& guid = ButtonGuid[result.PressedButton];
            if (ActivedNode < NodePool.size())
            {
                InputNodeUI& node = NodePool[ActivedNode];;
                node.Show(result.PressedButton);
                if (Horizontal)
                {
                    Transform& parent = Horizontal->transform;
                    node.SetParent(&parent);
                }
                ++ActivedNode;
            }
        }
    }
}