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

    bool NewFileWithDialog();
    bool LoadFileWithDialog();
    bool SaveFileWithDialog();

private:
    void ShowSystemDetail();
    void ShowTrackDetail();
    void ShowFilePathFrame();
    void ShowSequencerFrame();
    void ShowPreviewFrame();

    void DrawPreview();

    void DrawJudgeRange(std::pair<float, float> range, float circleRadius, ImU32 judgeCol, ImU32 bgCol = UINT_MAX - 1);
    void DrawNote(Timeline::EventContext* context, float circleRadius, ImColor noteCol, ImColor bgCol);

private:
    float CalcNoteAlphaFromPositionX(float posX);

private:
    QTE::Track                _qteTrack;                // QTE 트랙
    bool                      _editorOpened = false;    // 에디터 열림 여부
    EditorDragState           _dragHandler;             // 드래그 핸들러
    Timeline::SequencerEditor _sequencerEditor;         // 시퀀서 에디터

    float _PerfectTimer     = 0.0f;  // 퍼펙트 시 나오는 이펙트 타이머
    float _delayTimer       = 0.0f;  // 딜레이 타이머
    float _previewTimer     = 0.0f;  // 미리보기 타이머
    bool  _isPreviewPlaying = false; // 미리보기 재생 여부
    
    std::unordered_set<int> _noteJudgeSet;

    static constexpr float PERFECT_EFFECT_TIME = 0.5f;  // 퍼펙트 시 나오는 이펙트 시간
/*
QTE 파일 포함 정보
1. QTE의 무기 타입 (검, 단검, 해머)
2. QTE 노트 배치 정보
3. QTE 노트당 무슨 무기 애니메이션을 쓸건지, 
4. 무기 애니메이션 이벤트 트랙(+사운드까지 넣어서)
5. 몇 초 후에 QTE를 재생할건지(대기시간)
*/
};
