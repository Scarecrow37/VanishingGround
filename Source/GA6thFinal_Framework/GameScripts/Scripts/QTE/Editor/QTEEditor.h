#pragma once
#include <QTE/Track/QTETrack.h>

class WeaponElement;

class QTEEditor
{
public:
    QTEEditor();
    ~QTEEditor();

public:
    void Show();
    void Open();

    bool NewFileWithDialog(QTE::Track* qteTrack);
    bool LoadFileWithDialog(QTE::Track* qteTrack);
    bool SaveFileWithDialog(QTE::Track* qteTrack);

private:
    void ShowSystemDetail();
    void ShowTrackDetail();
    void ShowTrackDetailMenu();
    void ShowEditFrame();
    void ShowSequencerFrame(std::shared_ptr<Timeline::EventTrack> track);
    void ShowPreviewFrame();

    void ShowTrackFromWeapon(const QTE::Track& qteTrack, const std::string& weaponName, int weaponID, int index);

    void ProcessInputEvent();

    void DrawPreview(QTE::Track& qteTrack);

    void DrawJudgeRange(QTE::Track& qteTrack, std::pair<float, float> range, ImU32 judgeCol, ImU32 bgCol = UINT_MAX - 1);
    void DrawNote(QTE::Track& qteTrack, Timeline::EventContext* context, float circleRadius, ImColor noteCol, ImColor bgCol);

private:
    float CalcNoteAlphaFromPositionX(float posX);

private:
    File::Path                _lastUsedPath;            // 기본 경로
    QTE::Track                _editTrack;               // 에딧 QTE 트랙
    QTE::Track*               _previewTrack = nullptr;  // QTE 트랙
    bool                      _editorOpened = false;    // 에디터 열림 여부
    EditorDragState           _dragHandler;             // 드래그 핸들러
    Timeline::SequencerEditor _sequencerEditor;         // 시퀀서 에디터

    std::string _copyBuffer; // 노트 복사 버퍼

    float _perfectTimer     = 0.0f;  // 퍼펙트 시 나오는 이펙트 타이머
    float _previewTimer     = 0.0f;  // 미리보기 타이머
    bool  _isPreviewPlaying = false; // 미리보기 재생 여부
    
    std::unordered_set<int> _noteJudgeSet;

    static constexpr float PERFECT_EFFECT_TIME = 0.5f;  // 퍼펙트 시 나오는 이펙트 시간
};