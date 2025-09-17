#pragma once
#include "UmFramework.h"
#include <QTE/Result/QTEResult.h>
#include <Utility/SingletonHelper.h>

namespace QTE
{
    class Track;
}
class QTESystem;
class OverlayPanel;
class ImageElement;

class QTEUIManager : public Component
{
    USING_PROPERTY(QTEUIManager)

public:
    QTEUIManager();
    ~QTEUIManager() override;

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _notePrefabGuid.ToPath().string(); }
    PROPERTY(FilePath)

    void OnQTEEnter();
    void OnQTENotePressed(QTE::ResultType result);
    void OnQTEStay();
    void OnQTEExit();

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    void SetNotePrefabGuid(const File::Guid& guid);
    void SetUIAlpha(float factor);

private:
    void UpdateUITransformData();
    void FindUIComponents();
    void SpawnQTENotesFromCurrentTrack();
    void ClearAllQTENotes();

    float CalculateNotePosXFactor(float noteTime, float totalTime);
    float CalculateNotePosXAbsolute(float noteTime, float totalTime);
    float CalculateNotePosXAbsolute(float posFactor);
    float CalculateNoteAlpha(float posFactor);

    float GetJudgeNotePosXFactor() const;

    ImageElement* FindNoteUIFromNoteID(int noteID) const;

private:
    SingletonComponent<QTEUIManager>    _singletonComponent{this};
    QTESystem*                          _qteSystem          = nullptr;
    OverlayPanel*                       _qteOverlayPanel    = nullptr;
    ImageElement*                       _qteBackgroundUI    = nullptr;
    ImageElement*                       _qteNoteLineUI      = nullptr;
    ImageElement*                       _qteJudgeNoteUI     = nullptr;

    File::GuidRef                       _notePrefabGuid = File::NULL_GUID;
    std::unordered_map<int, ImageElement*> _noteSpawnTable = {};

    Vector2 _qtePanelPos  = Vector2::Zero;
    Vector2 _qtePanelSize = Vector2::Zero;
    Vector2 _qteJudgePos  = Vector2::Zero;
    Vector2 _qteJudgeSize = Vector2::Zero;

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    REFLECT_FIELDS_END(QTEUIManager)

    float _qteUIAlphaFactor = 0.0f;
};
