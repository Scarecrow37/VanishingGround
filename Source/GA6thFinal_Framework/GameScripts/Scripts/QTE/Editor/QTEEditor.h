#pragma once
#include <QTE/Track/QTETrack.h>

class QTEEditor
{
public:
    QTEEditor();
    ~QTEEditor();

public:
    void Show();
    void Open();

private:
    QTE::Track                _qteTrack;                // QTE 트랙
    bool                      _editorOpened = false;    // 에디터 열림 여부
    EditorDragState           _dragHandler;             // 드래그 핸들러
    Timeline::SequencerEditor _sequencerEditor;         // 시퀀서 에디터

/*
QTE 파일 포함 정보
1. QTE의 무기 타입 (검, 단검, 해머)
2. QTE 노트 배치 정보
3. QTE 노트당 무슨 무기 애니메이션을 쓸건지, 
4. 무기 애니메이션 이벤트 트랙(+사운드까지 넣어서)
5. 몇 초 후에 QTE를 재생할건지(대기시간)
*/
};
