#pragma once
#include "UmFramework.h"
#include "Utility/SingletonHelper.h"

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
    void OnQTEFail();
    void OnQTESuccess();
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

private:
    void FindUIComponents();
    void SpawnQTENotesFromCurrentTrack();
    void ClearAllQTENotes();

    float CalculateNotePosXFactor(float noteTime, float totalTime);
    float CalculateNoteAlpha(float posFactor);

    ImageElement* FindNoteUIFromNoteID(int noteID) const;

private:
    SingletonComponent<QTEUIManager>    _singletonComponent{this};
    QTESystem*                          _qteSystem = nullptr;
    OverlayPanel*                       _qteOverlayPanel = nullptr;
    ImageElement*                       _qteBackGroundUI = nullptr;
    ImageElement*                       _qteJudgeNoteUI = nullptr;

    File::GuidRef                       _notePrefabGuid = File::NULL_GUID;
    std::unordered_map<int, ImageElement*> _noteSpawnTable = {};

    POINT   _qtePanelPos  = {0, 0};
    SIZE    _qtePanelSize = {0, 0};

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    REFLECT_FIELDS_END(QTEUIManager)

};
