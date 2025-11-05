#pragma once
#include <QTE/Result/QTEResult.h>
#include <Utility/SingletonHelper.h>
#include <Utility/FadeHelper.h>

#include "QTE/UI/Background/QTEBackgroundUI.h"
#include "QTE/UI/Input/QTEInputViewerUI.h"
#include "QTE/UI/Input/QTEInputNodeUI.h"
#include "QTE/UI/Field/QTEFieldUI.h"
#include "QTE/UI/Guide/QTEGuideUI.h"

#include "QTE/UI/Guide/BattleGuideUI.h"

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
    REFLECT_PROPERTY(NotePrefab, EffectPrefab, ButtonPrefab, PoolSize)

    GETTER_ONLY(std::string, NotePrefab) { return File::Guid(ReflectFields->NotePrefabGuid).ToPath().string(); }
    PROPERTY(NotePrefab)

    GETTER_ONLY(std::string, EffectPrefab) { return File::Guid(ReflectFields->EffectPrefabGuid).ToPath().string(); }
    PROPERTY(EffectPrefab)

    GETTER_ONLY(std::string, ButtonPrefab) { return File::Guid(ReflectFields->ButtonPrefabGuid).ToPath().string(); }
    PROPERTY(ButtonPrefab)

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
    void SetQTEProgress(float t);

    /// <summary>QTE UI의 알파 값을 설정합니다.</summary>
    void SetUIAlpha(float factor);

    void DrawDebugJudgeLine();

    void SetDefaultState();

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;
    bool DragDropEvent(File::Guid& out);

private:
    void ResetUI();
    int  GetIndexFromNoteID(UINT id);
    void FindUIComponents();

private:
    SingletonComponent<QTEUIManager>    _singletoneComponent{this};

    OverlayPanel*                       _overlayPanel = nullptr;
    QTE::BackgroundUI                   _backGroundUI;
    QTE::InputViewerUI                  _inputViewerUI;
    QTE::FieldUI                        _fieldUI;
    QTE::GuideUI                        _guideUI;

    BattleGuideUI                       _battleGuideUI;

    std::unordered_map<UINT, int>       _activedPoolIndices;

    Fader _mainFader;

    REFLECT_FIELDS_BEGIN(Component)
    int         PoolSize = 15;
    std::string NotePrefabGuid;     // QTE 노트 프리팹 GUID
    std::string EffectPrefabGuid;   // QTE 이펙트 프리팹 GUID
    std::string ButtonPrefabGuid;   // QTE 입력 버튼 프리팹 GUID
    REFLECT_FIELDS_END(QTEUIManager)
    
};