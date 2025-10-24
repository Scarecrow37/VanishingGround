#pragma once
#include "DLLExportDefine.h"
#include "QTE/Common/QTECommon.h"
#include "QTE/Callback/Callback.h"
#include "QTE/Result/QTEResult.h"
#include "QTE/KeyBinder/QTEKeyBinder.h"
#include "QTE/Track/QTETrack.h"
#include "Utility/SingletonHelper.h"

class QTEUIManager;
class QTEEditor;
namespace QTE
{
    class Track;
    class Note;
}

struct WeaponStats;

class QTESystem : public Component, public InputReceiver
{
    using ControllerState = std::pair<const Input::Controller*, Input::ControllerTypes::Button>;

    friend class QTEUIManager;
    USING_PROPERTY(QTESystem)

public:
    QTESystem();
    virtual ~QTESystem();

    REFLECT_PROPERTY(ScaledSpeedFactor, CurrentTrackTime, MaxTracktime)

    GETTER_ONLY(float, ScaledSpeedFactor) {
        const float systemSpeedScale = ReflectFields->QTESpeedScale;
        const float trackSpeedScale  = _currentQTETrack ? _currentQTETrack->GetQTESpeedScale() : 1.0f;
        return systemSpeedScale * trackSpeedScale;
    }
    PROPERTY(ScaledSpeedFactor)

    GETTER_ONLY(float, CurrentTrackTime) { return _qteTimer; }
    PROPERTY(CurrentTrackTime)

    GETTER_ONLY(float, MaxTracktime) { return _qteMaxTime; }
    PROPERTY(MaxTracktime)
private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;
    void OnDestroy() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    /// <summary>QTE를 시작합니다. 현재 무기에 맞는 트랙이 있는 경우 트랙으로, 없으면 무기 기반으로 재생합니다.</summary>
    void StartQTE();
    /// <summary>트랙 기반으로 QTE를 시작합니다.</summary>
    void StartQTE(QTE::Track* qteTrack);
    /// <summary>무기 기반으로 임의의 랜덤 트랙을 생성하여 QTE를 시작합니다.</summary>
    void StartQTE(const WeaponStats* weapon);
    /// <summary>QTE를 중지합니다.</summary>
    void StopQTE();
    /// <summary>QTE를 일시정지하거나 재개합니다. QTE플레이 중이 아니라면 무시됩니다.</summary>
    void PauseQTE(bool pause);

    /// <summary>QTE 키 바인드 상태를 초기화합니다.</summary>
    void ClearKeyBindState();
    /// <summary>QTE 키 바인드 상태를 Push합니다.</summary>
    void PushKeyBindState(const QTE::KeyBindState& bindState);
    /// <summary>QTE 키 바인드 상태를 Pop합니다.</summary>
    void PopKeyBindState();
    /// <summary>현재 QTE 키 바인드 상태를 반환합니다.</summary>
    const QTE::KeyBindState& GetCurrentKeyBindState();

    /// <summary>QTE 콜백을 등록합니다.</summary>
    QTE::Callback::Handle RegisterCallback(const QTE::Callback& callback);
    /// <summary>QTE 콜백을 해제합니다.</summary>
    bool UnRegisterCallback(QTE::Callback::Handle handle);

    inline QTE::PlayState GetPlayState() { return _currState; }

private:
    void ResetQTEState();
    void ClearTrack();
    void ClearQueue();
    void UpdateQTETrack();
    QTE::ResultType GetQTEResult(float noteTime);

private:
    bool CanPressQTEButton();
    bool CanPressQTEButton(float noteTime);
    void PressedQTEButton(Input::Controller::Button buttonType = Input::Controller::Button::UNDEFINED);
    void PressedButtonX(const Input::Controller& controller);
    void PressedButtonY(const Input::Controller& controller);
    void PressedButtonB(const Input::Controller& controller);

    void ProcessQTEEnterEvent();
    void ProcessQTEExitEvent();
    void ProcessQTEFadeInEndEvent();
    void ProcessQTEFadeOutEndEvent();
    void ProcessQTEPlayingEvent();
    void ProcessQTEButtonPressedEvent();
    void ProcessQTENotePressedEvent(UINT noteID, QTE::ResultType result);

public:
    inline bool  IsQTEPlaying() const { return _currState != QTE::STATE_WAITING; }
    inline float GetQTETime() const { return _qteTimer; }

    inline void  SetQTESpeedScale(float scale) { ReflectFields->QTESpeedScale = scale; }
    inline float GetQTESpeedScale() const { return ReflectFields->QTESpeedScale; }
    inline void  SetDelayFromQTEStart(float delay) { ReflectFields->DelayFromQTEStart = delay; }
    inline float GetDelayFromQTEStart() const { return ReflectFields->DelayFromQTEStart; }

    inline void SetValidJudgeRange(float start, float end) { ReflectFields->ValidJudgeRange = {start, end}; }
    inline std::pair<float, float> GetValidJudgeRange() const { return ReflectFields->ValidJudgeRange; }
    inline void SetPerfectJudgeRange(float start, float end) { ReflectFields->PerfectJudgeRange = {start, end}; }
    inline std::pair<float, float> GetPerfectJudgeRange() const { return ReflectFields->PerfectJudgeRange; }
    inline void SetNormalJudgeRange(float start, float end) { ReflectFields->NormalJudgeRange = {start, end}; }
    inline std::pair<float, float> GetNormalJudgeRange() const { return ReflectFields->NormalJudgeRange; }

    inline void SetFadeInPosFactor(float start, float end) { ReflectFields->FadeInPosFactor = {start, end}; }
    inline std::pair<float, float> GetFadeInPosFactor() const { return ReflectFields->FadeInPosFactor; }
    inline void SetFadeOutPosFactor(float start, float end) { ReflectFields->FadeOutPosFactor = {start, end}; }
    inline std::pair<float, float> GetFadeOutPosFactor() const { return ReflectFields->FadeOutPosFactor; }

    inline QTE::Track* GetCurrentQTETrack() { return _currentQTETrack; }
    inline QTE::OverallResult& GetQTEOverallResult() { return _overallResult; }
    inline const std::vector<QTE::NoteData>& GetCurrentQTEAvailQueue() const { return _noteAvailQueue; }
    inline const std::unordered_map<int, std::vector<QTE::Track*>>& GetWeaponIDToTrackTable() const { return _weaponIDToTrackTable; }

    inline size_t GetCurrentNoteIndex() const { return _currentNoteIndex; }

    /// <summary>무기 ID에 매핑 트랙을 추가합니다. path인자를 NULL_PATH로 지정하면 빈 트랙을 생성합니다.</summary>
    /// <param name="weaponID">매핑 트랙을 추가할 무기의 ID입니다.</param>
    /// <param name="path">추가할 트랙의 파일 경로입니다.</param>
    QTE::Track* AddMappingTrackToWeaponID(int weaponID, const File::Path& path = File::NULL_PATH);

    /// <summary>weaponID와 관련된 매핑 트랙을 제거합니다. index가 -1이면 마지막 매핑 트랙을 제거합니다.</summary>
    /// <param name="weaponID">매핑을 제거할 무기의 ID입니다.</param>
    /// <param name="index">제거할 매핑 트랙의 인덱스입니다. 기본값은 -1로, 마지막 매핑 트랙을 제거합니다.</param>
    bool RemoveMappingTrackToWeaponID(int weaponID, int index = -1);

    /// <summary>weaponID와 선택적 인덱스에 해당하는 매핑된 트랙을 반환합니다.</summary>
    /// <param name="weaponID">매핑할 무기의 ID입니다.</param>
    /// <param name="index">매핑된 트랙의 인덱스(기본값은 0)입니다.</param>
    QTE::Track* GetMappingTrackToWeaponID(int weaponID, int index = 0);

private:
    SingletonComponent<QTESystem> _singletonComponent{this};
    std::unordered_map<int, std::vector<QTE::Track*>> _weaponIDToTrackTable; // 무기 ID QTE 매핑 테이블

    QTE::Track*                 _currentQTETrack  = nullptr;
    size_t                      _currentNoteIndex = 0;              // 현재 가리키는 노트 인덱스
    std::vector<QTE::NoteData>  _noteAvailQueue;                    // 유효한 노트 큐

    QTE::PlayState              _currState = QTE::STATE_WAITING;    // QTE 현재 상태
    QTE::PlayState              _prevState = QTE::STATE_WAITING;    // QTE 이전 상태
    QTE::KeyBinder              _keyBinder;                         // QTE 키 바인딩 처리
    QTE::CallbackHandler        _callbackHandler;                   // QTE 콜백 처리
    QTE::OverallResult          _overallResult;                     // QTE 최종 결과
    ControllerState             _nextKeyEvent = {nullptr, Input::ControllerTypes::UNDEFINED};

    float                       _qteTimer           = 0.0f;                     // QTE 타이머
    float                       _qteMaxTime         = 0.0f;                     // QTE 최대 시간
    bool                        _qtePaused          = false;                    // QTE 일시정지 여부

    REFLECT_FIELDS_BEGIN(Component)
    float                   QTESpeedScale       = 1.0f;                         // QTE 속도 배율
    float                   DelayFromQTEStart   = 0.0f;                         // QTE 시작 대기 시간
    std::pair<float, float> PerfectJudgeRange   = {-0.05f, 0.05f};              // 퍼펙트 판정 범위 (min - max)
    std::pair<float, float> NormalJudgeRange    = {-0.1f, 0.1f};                // 노멀 판정 범위 (min - max)
    std::pair<float, float> ValidJudgeRange     = {-0.3f, 0.3f};                // 유효 판정 범위 (min - max)
    std::pair<float, float> FadeInPosFactor     = {0.0f, 0.0f};                 // 페이드인 위치 비율 (0 ~ 1)
    std::pair<float, float> FadeOutPosFactor    = {1.0f, 1.0f};                 // 페이드아웃 위치 비율 (0 ~ 1)

    std::unordered_map<int, std::vector<std::string>> WeaponQTETrackGuids;      // 무기 ID 별 QTE 트랙 파일 Guid
    REFLECT_FIELDS_END(QTESystem)

    // QTE 편집기
    QTEEditor&  GetEditor();

    inline static constexpr Input::ControllerTypes::Vibration PERFECT_VIBRATION{
        .LeftMotorSpeed  = (unsigned short)(0.8f * 65535.0f),
        .RightMotorSpeed = (unsigned short)(1.0f * 65535.0f), 
        .Duration = std::chrono::milliseconds(220)};

    inline static constexpr Input::ControllerTypes::Vibration NORMAL_VIBRATION{
        .LeftMotorSpeed  = (unsigned short)(0.3f * 65535.0f),
        .RightMotorSpeed = (unsigned short)(0.5f * 65535.0f),
        .Duration        = std::chrono::milliseconds(150)};

    inline static constexpr Input::ControllerTypes::Vibration MISS_VIBRATION{
        .LeftMotorSpeed  = (unsigned short)(0.2f * 65535.0f),
        .RightMotorSpeed = (unsigned short)(0.7f * 65535.0f),
        .Duration        = std::chrono::milliseconds(150)};
};
