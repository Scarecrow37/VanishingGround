#pragma once
#include "QTEInputNodeUI.h"
#include "QTE/Result/QTEResult.h"

class OverlayPanel;
class HorizontalPanel;
class ImageElement;

namespace QTE
{
    struct InputViewerUI
    {
        OverlayPanel*               Overlay     = nullptr;
        HorizontalPanel*            Horizontal  = nullptr;
        std::vector<InputNodeUI>    NodePool;
        size_t                      ActivedNode = 0;

        std::unordered_map<NoteResult::Button, File::Guid> ButtonGuid;

        void Initialize(File::Guid prefabGuid, size_t poolSize);

        // 해당 오브젝트에 자신의 멤버가 있으면 매칭합니다.
        void MatchUIFromObject(GameObject& object);

        // 상위 오브젝트의 활성화 여부를 조작합니다.
        void Active(bool active);

        // UI 알파를 적용합니다.
        void Alpha(float alpha);

        // UI 상태를 초기화합니다.
        void Reset();

        void OnNotePressed(const QTE::NoteResult& resultType);

    private:
        static constexpr const char* OVERLAY_TAG    = "QTE Input Viewer";
        static constexpr const char* HORIZONTAL_TAG = "QTE Input Horizontal";
    };
}
