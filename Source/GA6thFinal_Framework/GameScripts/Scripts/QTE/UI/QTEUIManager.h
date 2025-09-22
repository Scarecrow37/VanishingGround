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
    inline static QTEUIManager* _staticInstance;
    inline static QTEUIManager* GetInstance() { return _staticInstance; }

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _notePrefabGuid.ToPath().string(); }
    PROPERTY(FilePath)

    void OnQTEEnter();
    void OnQTENotePressed(QTE::ResultType result);
    void OnQTEStay();
    void OnQTEExit();

public:
    /// <summary>노트 프리팹의 GUID를 설정합니다. 해당 프리팹을 통해 QTE 노트 UI가 생성됩니다.</summary>
    /// <param name="guid">설정할 File::Guid 객체입니다.</param>
    void SetNotePrefabGuid(const File::Guid& guid);

    /// <summary>QTE 백그라운드 UI의 알파 값을 설정합니다.</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetBackgroundUIAlpha(float factor);

    /// <summary>QTE 관련 UI의 알파 값을 설정합니다. (백그라운드는 제외입니다.)</summary>
    /// <param name="factor">설정할 UI의 알파 값입니다. 0.0f에서 1.0f 사이의 값을 가집니다.</param>
    void SetUIAlpha(float factor);

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
    void UpdateUITransformData();
    bool CheckUIValid();
    void FindUIComponents();
    void SpawnQTENotesFromCurrentTrack();
    void ClearAllQTENotes();

    float CalculateNotePosXFactor(float noteTime, float totalTime);
    float CalculateNotePosX(float noteTime, float totalTime);
    float CalculateNotePosX(float posFactor);
    float CalculateNoteAlpha(float posFactor);

    ImageElement* FindNoteUIFromNoteID(int noteID) const;

private:
    OverlayPanel*   _qteOverlayPanel    = nullptr;
    ImageElement*   _qteBackgroundUI    = nullptr;
    ImageElement*   _qteNoteLineUI      = nullptr;
    ImageElement*   _qteJudgeNoteUI     = nullptr;
    File::GuidRef   _notePrefabGuid     = File::NULL_GUID;
    std::unordered_map<int, ImageElement*> _noteSpawnTable = {};

    Vector2 _qtePanelPos  = Vector2::Zero;
    Vector2 _qtePanelSize = Vector2::Zero;
    Vector2 _qteJudgePos  = Vector2::Zero;
    Vector2 _qteJudgeSize = Vector2::Zero;

    REFLECT_FIELDS_BEGIN(Component)
    std::string NotePrefabGuid; // QTE 노트 프리팹 GUID
    REFLECT_FIELDS_END(QTEUIManager)

    class Fader
    {
    public:
        enum Mode { FADE_NONE, FADE_IN, FADE_OUT };

        Fader() = default;
        ~Fader() = default;
        
    public:
        /// <summary>모드에 따라 페이드를 갱신합니다. 0~1 사이의 Factor를 반환합니다.</summary>
        float Fade();
        /// <summary>페이드 인이 종료되었는지 반환합니다.</summary>
        bool IsFadeInEnd() const;
        /// <summary>페이드 아웃이 종료되었는지 반환합니다.</summary>
        bool IsFadeOutEnd() const;

    public:
        /// <summary>
        /// 페이드 인이 끝났을 때 호출될 콜백 함수를 설정합니다. 생명관리를 하지 않으므로 사용 시 주의해야 합니다.
        /// </summary>
        /// <param name="callback">페이드 인 종료 시 실행할 콜백 함수입니다.</param>
        void SetOnFadeInEndCallback(const std::function<void()>& callback);

        /// <summary>
        /// 페이드 아웃이 끝날 때 호출될 콜백 함수를 설정합니다. 생명관리를 하지 않으므로 사용 시 주의해야 합니다.
        /// </summary>
        /// <param name="callback">페이드 아웃 종료 시 실행할 콜백 함수입니다.</param>
        void SetOnFadeOutEndCallback(const std::function<void()>& callback);

        /// <summary>
        /// 페이드 모드를 설정합니다.
        /// </summary>
        /// <param name="mode">설정할 페이드 모드입니다.</param>
        void SetFadeMode(Mode mode);

        /// <summary>
        /// 지정된 지속 시간을 설정합니다.
        /// </summary>
        /// <param name="duration">설정할 지속 시간(초 단위)입니다.</param>
        void SetDuration(float duration);

        /// <summary>
        /// 타이머 값을 설정합니다. 0부터 duration 사이의 값을 클램핑합니다.
        /// </summary>
        /// <param name="timer">설정할 타이머 값(초 단위)입니다.</param>
        void SetTimer(float timer);

        /// <summary>
        /// 페이드 인 효과의 타입과 함수 유형을 설정합니다.
        /// </summary>
        /// <param name="type">적용할 페이드 인 이징 타입입니다.</param>
        /// <param name="func">사용할 이징 함수 유형입니다.</param>
        void SetFadeInType(Mathf::EaseType type, Mathf::EaseFuncType func);

        /// <summary>
        /// 페이드 아웃 효과의 타입과 함수 유형을 설정합니다.
        /// </summary>
        /// <param name="type">적용할 페이드 아웃 이징 타입입니다.</param>
        /// <param name="func">적용할 이징 함수 유형입니다.</param>
        void SetFadeOutType(Mathf::EaseType type, Mathf::EaseFuncType func);

        inline Mode                 GetFadeMode() const { return _fadeMode; }
        inline float                GetFadeFactor() const { return _fadeFactor; }
        inline float                GetDuration() const { return _duration; }
        inline Mathf::EaseType      GetFadeInEaseType() const { return _fadeInEaseType; }
        inline Mathf::EaseFuncType  GetFadeInFuncType() const { return _fadeInFuncType; }
        inline Mathf::EaseType      GetFadeOutEaseType() const { return _fadeOutEaseType; }
        inline Mathf::EaseFuncType  GetFadeOutFuncType() const { return _fadeOutFuncType; }

    private:
        /// <summary>FadeIn을 갱신합니다. 0~1 사이의 Factor를 반환합니다.</summary>
        float FadeIn();
        /// <summary>FadeOut을 갱신합니다. 0~1 사이의 Factor를 반환합니다.</summary>
        float FadeOut();

    private:
        float _duration   = 0.0f;
        float _timer      = 0.0f;
        float _fadeFactor = 0.0f;
        Mode  _fadeMode   = FADE_NONE;

        Mathf::EaseType     _fadeInEaseType = Mathf::EASE_IN;
        Mathf::EaseFuncType _fadeInFuncType = Mathf::SINE;
        Mathf::EaseType     _fadeOutEaseType = Mathf::EASE_OUT;
        Mathf::EaseFuncType _fadeOutFuncType = Mathf::SINE;

        std::function<void()> _onFadeInEndCallback = nullptr;
        std::function<void()> _onFadeOutEndCallback = nullptr;
    };
    Fader _fader;
};