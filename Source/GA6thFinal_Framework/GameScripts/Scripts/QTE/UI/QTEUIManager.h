#pragma once
#include <QTE/Result/QTEResult.h>
#include <Utility/SingletonHelper.h>
#include <Utility/FadeHelper.h>

#include "QTE/UI/Background/QTEBackgroundUI.h"
#include "QTE/UI/Input/QTEInputViewerUI.h"
#include "QTE/UI/Input/QTEInputNodeUI.h"
#include "QTE/UI/Field/QTEFieldUI.h"
#include "QTE/UI/Guide/QTEGuideUI.h"
#include "QTE/UI/Note/QTENoteUI.h"

class SpriteAnimationElement;

namespace QTE
{
    class Track;
}
class QTESystem;
class OverlayPanel;
class ImageElement;
class CameraComponent;

/// <summary>
/// QTE UI Manager는 QTE System, CombatStartPhase와 PlayerPlayTurn에서만 사용합니다.
/// 이 외에는 접근하지 마세요.
/// </summary>
class QTEUIManager : public Component
{
    friend class QTESystem;
    USING_PROPERTY(QTEUIManager)

public:
    QTEUIManager();
    ~QTEUIManager() override;

public:
    REFLECT_PROPERTY(FilePath, PoolSize)
    GETTER_ONLY(std::string, FilePath) { return File::Guid(ReflectFields->NotePrefabGuid).ToPath().string(); }
    PROPERTY(FilePath)

    SETTER(int, PoolSize) { ReflectFields->PoolSize = value; }
    GETTER(int, PoolSize) { return ReflectFields->PoolSize; }
    PROPERTY(PoolSize)

private:
    void OnQTEEnter();
    void OnQTEButtonPressed();
    void OnQTENotePressed(UINT noteID, const QTE::NoteResult& result);
    void OnQTEPlay();
    void OnQTEExit();

public:
    /// <summary>노트 프리팹의 GUID를 설정합니다. 해당 프리팹을 통해 QTE 노트 UI가 생성됩니다.</summary>
    void SetNotePrefabGuid(const File::Guid& guid);

    /// <summary>QTE UI의 알파 값을 설정합니다.</summary>
    void SetUIAlpha(float factor);

    void DrawDebugJudgeLine();

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void ResetUI();
    void InitializeNotePool();
    void InitializeInputNodePool();

    QTE::NoteUI* GetNoteUIFromID(UINT id);

    void FindUIComponents();

private:
    SingletonComponent<QTEUIManager>    _singletoneComponent{this};

    OverlayPanel*                       _overlayPanel = nullptr;
    QTE::BackgroundUI                   _backGroundUI;
    QTE::InputViewerUI                  _inputViewerUI;
    QTE::FieldUI                        _fieldUI;
    QTE::GuideUI                        _guideUI;

    std::vector<QTE::NoteUI>            _notePool;
    std::unordered_map<UINT, size_t>    _activedNote;

    Fader _mainFader;

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    int         PoolSize = 15;
    REFLECT_FIELDS_END(QTEUIManager)
    
};