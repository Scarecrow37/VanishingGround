#include "pchScripts.h"
#include "QTEEditor.h"

QTEEditor::QTEEditor() 
{
    _sequencerEditor.SetEventTrack(_qteTrack.GetEventTrack());
    _sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_ALL_INPUT);
    auto& callback           = _sequencerEditor.GetCallback();
    callback.LowerFramePopup = [this](Timeline::EventTrack* editor) {
        if (ImGui::MenuItem("Add Note"))
        {
            editor->AddEvent<QTE::Note>("Note", editor->GetCurrentFrame());
        }
    };
}

QTEEditor::~QTEEditor() 
{
}

void QTEEditor::Show()
{
    if (_editorOpened)
    {
        ImGui::Begin("QTE Editor", &_editorOpened, ImGuiWindowFlags_NoMove);
        ImRect tabRect  = ImGuiHelper::GetWindowTabBarRect();
        ImVec2 mousePos = ImGui::GetMousePos();

        int state = _dragHandler.BeginDragState("WindowTab", tabRect, mousePos, ImGuiMouseButton_Left);
        switch (state)
        {
        case EditorDragState::DRAG_STATE_DRAGGING: {
            ImVec2 newPos = tabRect.Min + ImGui::GetIO().MouseDelta;
            ImGui::SetWindowPos(newPos);
            break;
        }
        default:
            break;
        }

        _sequencerEditor.Show();
        ImGui::End();
    }
}

void QTEEditor::Open() 
{
    _editorOpened = true;
}