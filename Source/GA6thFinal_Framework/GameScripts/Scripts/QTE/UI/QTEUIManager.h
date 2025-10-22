#pragma once
#include "UmFramework.h"
#include <QTE/Result/QTEResult.h>
#include <Utility/SingletonHelper.h>
#include <Utility/FadeHelper.h>

class SpriteAnimationElement;

namespace QTE
{
    class Track;
}
class QTESystem;
class OverlayPanel;
class ImageElement;
class CameraComponent;

class QTEUIManager : public Component
{
    friend class QTESystem;
    USING_PROPERTY(QTEUIManager)

    struct QTENoteUI
    {
        OverlayPanel* Overlay;
        SpriteAnimationElement* Begin;
        SpriteAnimationElement* End;
    };

public:
    QTEUIManager();
    ~QTEUIManager() override;
    inline static QTEUIManager* _staticInstance;
    inline static QTEUIManager* GetInstance() { return _staticInstance; }

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _notePrefabGuid.ToPath().string(); }
    PROPERTY(FilePath)

private:
    void OnQTEEnter();
    void OnQTEButtonPressed() const;
    void OnQTENotePressed(UINT noteID, QTE::ResultType result);
    void OnQTEStay();
    void OnQTEExit();

public:
    /// <summary>
    /// UI데이터를 갱신합니다.
    /// </summary>
    void Refresh();

    /// <summary>노트 프리팹의 GUID를 설정합니다. 해당 프리팹을 통해 QTE 노트 UI가 생성됩니다.</summary>
    /// <param name="guid">설정할 File::Guid 객체입니다.</param>
    void SetNotePrefabGuid(const File::Guid& guid);

    // TODO: 나중에 블러로 변경 가능성 있음
    /// <summary>QTE 백그라운드 UI의 알파 값을 설정합니다.</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetBackgroundUIAlpha(float factor);

    /// <summary>QTE 가이드 노트의 알파 값을 설정합니다.</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetGuideNoteUIAlpha(float factor);
    void SetGuideNoteActive(bool active);

    /// <summary>QTE 관련 UI의 알파 값을 설정합니다. (백그라운드는 제외입니다.)</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetQTEBarUIAlpha(float factor);

    void SetQTEAnimBarUIAlpha(float factor);

    /// <summary>객체의 활성 상태를 설정합니다.</summary>
    /// <param name="active">객체를 활성화할지 여부를 지정하는 불리언 값입니다.</param>
    void SetActive(bool active);

    void StartShowQTEGuideNote();
    void StartHideQTEGuideNote();

    void UpdateQTEUI();
    void UpdateGuideNoteUI();

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnDestroy() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void StartBeginQTEAnimation() const;
    void RefreshQTEUITransformData();
    bool RefreshGuideNoteUITransformData();
    bool CheckUIValid();
    void FindUIComponents();
    void SpawnQTENotesFromCurrentTrack();
    void ClearAllQTENotes();
    void ClearAllEffects();
    void SpawnJudgmentEffect(const SpriteAnimationElement* originalEffectUI, POINT position);

    float CalculateNotePosXFactor(float noteTime, float totalTime);
    float CalculateNotePosX(float noteTime, float totalTime);
    float CalculateNotePosX(float posFactor);
    float CalculateNoteAlpha(float posFactor);

    QTENoteUI FindNoteUIFromNoteID(int noteID) const;

private:
    OverlayPanel*                          _qteOverlayPanel                  = nullptr;
    ImageElement*                          _qteBackgroundUI                  = nullptr;
    ImageElement*                          _qteNoteLineUI                    = nullptr;
    SpriteAnimationElement*                _qteJudgeNoteUI                   = nullptr;
    SpriteAnimationElement*                _qteStartAnimationUI              = nullptr;
    SpriteAnimationElement*                _qteOriginalJudgmentPerfectEffect = nullptr;
    SpriteAnimationElement*                _qteOriginalJudgmentGoodEffect    = nullptr;
    SpriteAnimationElement*                _qteOriginalJudgmentMissEffect    = nullptr;
    SpriteAnimationElement*                _qteFlow                          = nullptr;
    File::Guid   _notePrefabGuid     = File::NULL_GUID;
    std::unordered_map<int, QTENoteUI>    _noteSpawnTable                   = {};
    std::vector<SpriteAnimationElement*>   _activeJudgmentEffects            = {};

    Vector2 _qtePanelPos  = Vector2::Zero;
    Vector2 _qtePanelSize = Vector2::Zero;
    Vector2 _qteJudgePos  = Vector2::Zero;
    Vector2 _qteJudgeSize = Vector2::Zero;

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    float       GuideNoteDuration = 0.5f;
    REFLECT_FIELDS_END(QTEUIManager)

    
    Fader _mainFader;

    Fader         _xybAlphaFader;
    Fader         _xybPointFader;
    float         _xybOutTimer      = 0.0f;
    ImageElement* _qteGuideNoteX    = nullptr;
    ImageElement* _qteGuideNoteY    = nullptr;
    ImageElement* _qteGuideNoteB    = nullptr;
    Vector3       _enemyXPos        = Vector3::Zero;
    Vector3       _enemyYPos        = Vector3::Zero;
    Vector3       _enemyBPos        = Vector3::Zero;
};