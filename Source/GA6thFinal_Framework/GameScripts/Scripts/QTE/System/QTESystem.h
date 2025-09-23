#pragma once
#include "DLLExportDefine.h"
#include <QTE/Result/QTEResult.h>
#include "Utility/SingletonHelper.h"

class QTEUIManager;
class QTEEditor;
namespace QTE
{
    class Track;
    class Note;
} 

/// <summary>
/// 콜백으로 책임회피?
/// </summary>
class QTESystem : public Component, public InputReceiver
{

    friend class QTEUIManager;
    USING_PROPERTY(QTESystem)

public:
    QTESystem();
    virtual ~QTESystem();

private:
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    /// <summary>
    /// 무기 ID에 매핑 트랙을 추가합니다. path인자를 NULL_PATH로 지정하면 빈 트랙을 생성합니다.
    /// </summary>
    /// <param name="weaponID">매핑 트랙을 추가할 무기의 ID입니다.</param>
    /// <param name="path">추가할 트랙의 파일 경로입니다.</param>
    /// <returns>매핑 트랙 추가가 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool AddMappingTrackToWeaponID(int weaponID, const File::Path& path = File::NULL_PATH);

    /// <summary>
    /// weaponID와 관련된 매핑 트랙을 제거합니다. index가 -1이면 마지막 매핑 트랙을 제거합니다.
    /// </summary>
    /// <param name="weaponID">매핑을 제거할 무기의 ID입니다.</param>
    /// <param name="index">제거할 매핑 트랙의 인덱스입니다. 기본값은 -1로, 마지막 매핑 트랙을 제거합니다.</param>
    /// <returns>매핑이 성공적으로 제거되면 true를 반환하고, 그렇지 않으면 false를 반환합니다.</returns>
    bool RemoveMappingTrackToWeaponID(int weaponID, int index = -1);

    /// <summary>
    /// weaponID와 선택적 인덱스에 해당하는 매핑된 트랙을 반환합니다.
    /// </summary>
    /// <param name="weaponID">매핑할 무기의 ID입니다.</param>
    /// <param name="index">매핑된 트랙의 인덱스(기본값은 0)입니다.</param>
    /// <returns>QTE::Track 객체에 대한 포인터를 반환합니다. 해당 weaponID와 인덱스에 매핑된 트랙이 없으면 nullptr일 수 있습니다.</returns>
    QTE::Track* GetMappingTrackToWeaponID(int weaponID, int index = 0);

    /// <summary>
    /// QTE(퀵 타임 이벤트)를 시작하며, 선택적으로 QTE종료 시 콜백 함수를 실행합니다.
    /// </summary>
    /// <param name="callback">QTE가 종료되었을 때 호출되는 선택적 콜백 함수입니다. 기본값은 nullptr입니다.</param>
    void StartQTE(QTE::Result::Callback callback = nullptr);
    void StartQTE(QTE::Track* qteTrack, QTE::Result::Callback callback = nullptr);

    /// <summary>
    /// QTE를 일시정지하거나 재개합니다. QTE플레이 중이 아니라면 무시됩니다.
    /// </summary>
    /// <param name="pause"></param>
    void PauseQTE(bool pause);

    /// <summary>
    /// 전투 UI의 활성화 상태를 설정합니다.
    /// </summary>
    /// <param name="active">전투 UI를 활성화할지 여부를 지정하는 불리언 값입니다.</param>
    void CombatUIActive(bool active);

private:
    void ClearTrack();
    void ClearQueue();
    void UpdateQTETrack();
    QTE::ResultType GetQTEResult(QTE::Note* note);

private:
    bool CanPressQTEButton();
    bool CanPressQTEButton(QTE::Note* note);
    void PressedQTEButton(Input::Controller::Button buttonType = Input::Controller::Button::UNDEFINED);
    void PressedButtonX(const Input::Controller& controller);
    void PressedButtonY(const Input::Controller& controller);
    void PressedButtonB(const Input::Controller& controller);

    void ProcessQTEEnterEvent();
    void ProcessQTENotePressedEvent(QTE::ResultType result);
    void ProcessQTEStayEvent();
    void ProcessQTEExitEvent();
    void ProcessQTEFadeInEndEvent();
    void ProcessQTEFadeOutEndEvent();


public:
    inline bool  IsQTEPlaying() const { return _currQTEPlaying; }
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

    inline QTE::Track* GetCurrentQTETrack() const { return _currentQTETrack; }
    inline const std::vector<QTE::Result>& GetCurrentQTEResultQueue() const { return _noteResultQueue; }
    inline const std::vector<QTE::Note*>&  GetCurrentQTEAvailQueue() const { return _noteAvailQueue; }
    inline const std::unordered_map<int, std::vector<QTE::Track*>>& GetWeaponIDToTrackTable() const { return _weaponIDToTrackTable; }

    inline size_t GetCurrentNoteIndex() const { return _currentNoteIndex; }

private:
    SingletonComponent<QTESystem> _singletonComponent{this};

    std::unordered_map<int, std::vector<QTE::Track*>> _weaponIDToTrackTable;    // 무기 ID QTE 매핑 테이블

    QTE::Track*                 _currentQTETrack  = nullptr;                    // QTE 트랙
    size_t                      _currentNoteIndex = 0;                          // 현재 가리키는 노트 인덱스
    std::vector<QTE::Note*>     _noteAvailQueue;                                // 유효한 노트 큐
    std::vector<QTE::Result>    _noteResultQueue;                               // 노트 결과 큐

    float                       _qteTimer       = 0.0f;                         // QTE 타이머
    bool                        _qteFadeInEnd   = false;                        // QTE 페이드 인 종료 여부
    bool                        _qteFadeOutEnd  = false;                        // QTE 페이드 아웃 종료 여부
    bool                        _qtePaused      = false;                        // QTE 일시정지 여부
    bool                        _currQTEPlaying = false;                        // 현재 QTE가 실행 중인지 여부
    bool                        _prevQTEPlaying = false;                        // 이전 프레임에서 QTE가 실행 중이었는지 여부

    QTE::Result::Callback _onQTEFinishCallback = nullptr; // QTE 페이드 인 종료 콜백

    REFLECT_FIELDS_BEGIN(Component)
    float                   QTESpeedScale       = 1.0f;                         // QTE 속도 배율
    float                   DelayFromQTEStart   = 0.0f;                         // QTE 시작 대기 시간
    std::pair<float, float> PerfectJudgeRange   = {-0.05f, 0.05f};              // 퍼펙트 판정 범위 (min - max)
    std::pair<float, float> NormalJudgeRange    = {-0.1f, 0.1f};                // 노멀 판정 범위 (min - max)
    std::pair<float, float> ValidJudgeRange     = {-0.3f, 0.3f};                // 유효 판정 범위 (min - max)
    std::pair<float, float> FadeInPosFactor     = {0.0f, 0.0f};                 // 페이드인 위치 비율 (0 ~ 1)
    std::pair<float, float> FadeOutPosFactor    = {1.0f, 1.0f};                 // 페이드아웃 위치 비율 (0 ~ 1)

    std::unordered_map<int, std::vector<std::string>> WeaponQTETrackData;       // 무기 ID 별 QTE 트랙 파일 경로
    REFLECT_FIELDS_END(QTESystem)

    // QTE 편집기
    QTEEditor&  GetEditor();
};
