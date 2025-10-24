#pragma once
#include <QTE/Result/QTEResult.h>
#include <Utility/SingletonHelper.h>
#include <Utility/FadeHelper.h>

#include "QTE/UI/QTEBackgroundUI.h"
#include "QTE/UI/QTEFieldUI.h"
#include "QTE/UI/QTENoteUI.h"

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

public:
    QTEUIManager();
    ~QTEUIManager() override;

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return File::Guid(ReflectFields->NotePrefabGuid).ToPath().string(); }
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

    /// <summary>QTE 관련 UI의 알파 값을 설정합니다. (백그라운드는 제외입니다.)</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetQTEBarUIAlpha(float factor);

    void SetQTEAnimBarUIAlpha(float factor);

    /// <summary>객체의 활성 상태를 설정합니다.</summary>
    /// <param name="active">객체를 활성화할지 여부를 지정하는 불리언 값입니다.</param>
    void SetActive(bool active);

    void UpdateQTEUI();

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void InitializeNotePool();
    void ResetNotePool();

    QTE::NoteUI* GetNoteUIFromID(UINT id);

    void StartBeginQTEAnimation() const;
    void FindUIComponents();

private:
    SingletonComponent<QTEUIManager> _singletoneComponent{this};

    QTE::BackgroundUI                       _backGroundUI;
    QTE::FieldUI                            _fieldUI;
    std::vector<QTE::NoteUI>                _notePool;
    std::unordered_map<UINT, size_t>        _activedNote;

    OverlayPanel*                           _qteOverlayPanel                  = nullptr;
    ImageElement*                           _qteBackgroundUI                  = nullptr;
    ImageElement*                           _qteNoteLineUI                    = nullptr;
    SpriteAnimationElement*                 _qteJudgeNoteUI                   = nullptr;
    SpriteAnimationElement*                 _qteStartAnimationUI              = nullptr;

    SpriteAnimationElement*                 _qteFlow                          = nullptr;
    std::unordered_map<int, QTE::NoteUI>    _noteSpawnTable                   = {};
    std::vector<SpriteAnimationElement*>    _activeJudgmentEffects            = {};

    Vector2 _qtePanelPos  = Vector2::Zero;
    Vector2 _qtePanelSize = Vector2::Zero;
    Vector2 _qteJudgePos  = Vector2::Zero;
    Vector2 _qteJudgeSize = Vector2::Zero;

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    int         PoolSize;
    REFLECT_FIELDS_END(QTEUIManager)

    
    Fader _mainFader;

    Fader         _xybAlphaFader;
    Fader         _xybPointFader;
    float         _xybOutTimer      = 0.0f;
    Vector3       _enemyXPos        = Vector3::Zero;
    Vector3       _enemyYPos        = Vector3::Zero;
    Vector3       _enemyBPos        = Vector3::Zero;
};