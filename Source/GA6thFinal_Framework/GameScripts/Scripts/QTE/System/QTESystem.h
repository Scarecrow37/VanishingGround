#pragma once
#include <QTE/Result/QTEResult.h>
#include <QTE/Editor/QTEPreviewer.h>

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
    USING_PROPERTY(QTESystem)

public:
    QTESystem();
    virtual ~QTESystem();

private:
    void Reset() override;
    void Awake() override;
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
    bool        AddMappingTrackToWeaponID(int weaponID, const File::Path& path = File::NULL_PATH);

    /// <summary>
    /// weaponID와 관련된 매핑 트랙을 제거합니다. index가 -1이면 마지막 매핑 트랙을 제거합니다.
    /// </summary>
    /// <param name="weaponID">매핑을 제거할 무기의 ID입니다.</param>
    /// <param name="index">제거할 매핑 트랙의 인덱스입니다. 기본값은 -1로, 마지막 매핑 트랙을 제거합니다.</param>
    /// <returns>매핑이 성공적으로 제거되면 true를 반환하고, 그렇지 않으면 false를 반환합니다.</returns>
    bool        RemoveMappingTrackToWeaponID(int weaponID, int index = -1);

    /// <summary>
    /// weaponID와 선택적 인덱스에 해당하는 매핑된 트랙을 반환합니다.
    /// </summary>
    /// <param name="weaponID">매핑할 무기의 ID입니다.</param>
    /// <param name="index">매핑된 트랙의 인덱스(기본값은 0)입니다.</param>
    /// <returns>QTE::Track 객체에 대한 포인터를 반환합니다. 해당 weaponID와 인덱스에 매핑된 트랙이 없으면 nullptr일 수 있습니다.</returns>
    QTE::Track* GetMappingTrackToWeaponID(int weaponID, int index = 0);

    void StartQTE();
    void StartQTE(QTE::Track* qteTrack);

private:
    void ResetQTETimer();

    void UpdateQTEDelay();
    void UpdateQTETrack();

    bool IsQTEDelayEnd();
    bool IsQTETimeEnd();

    QTE::ResultType GetQTEResult(QTE::Note* note);

private:
    void PressedQTEButton(Input::ControllerTypes::Button type = Input::ControllerTypes::X);
    void PressedButtonX(const Input::Controller& controller);
    void PressedButtonY(const Input::Controller& controller);
    void PressedButtonB(const Input::Controller& controller);

public:
    inline bool  IsQTEPlaying() const { return _isQTEPlaying; }
    inline float GetQTEDelayTime() const { return _delayTimer; }
    inline float GetQTETime() const { return _qteTimer; }

    inline void  SetQTESpeedScale(float scale) { ReflectFields->QTESpeedScale = scale; }
    inline float GetQTESpeedScale() const { return ReflectFields->QTESpeedScale; }
    inline void  SetDelayFromQTEStart(float delay) { ReflectFields->DelayFromQTEStart = delay; }
    inline float GetDelayFromQTEStart() const { return ReflectFields->DelayFromQTEStart; }
    inline void  SetJudgePosFactor(float factor) { ReflectFields->JudgePosFactor = factor; }
    inline float GetJudgePosFactor() const { return ReflectFields->JudgePosFactor; }

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

    std::vector<QTE::Note*>     _noteAvailQueue;                                // 유효한 노트 큐
    std::vector<QTE::Result>    _noteResultQueue;                               // 노트 결과 큐
    size_t                      _currentNoteIndex = 0;                          // 현재 가리키는 노트 인덱스
    QTE::Track*                 _currentQTETrack  = nullptr;                    // QTE 트랙

    float                       _delayTimer     = 0.0f;                         // 딜레이 타이머
    float                       _qteTimer       = 0.0f;                         // QTE 타이머
    bool                        _isQTEPlaying   = false;                        // QTE 실행 중 여부

    REFLECT_FIELDS_BEGIN(Component)
    float                   QTESpeedScale       = 1.0f;                         // QTE 속도 배율
    float                   DelayFromQTEStart   = 0.0f;                         // QTE 시작 대기 시간
    float                   JudgePosFactor      = 0.8f;                         // 퍼펙트 노트 위치 비율 (0 ~ 1)
    std::pair<float, float> PerfectJudgeRange   = {-0.05f, 0.05f};              // 퍼펙트 판정 범위 (min - max)
    std::pair<float, float> NormalJudgeRange    = {-0.1f, 0.1f};                // 노멀 판정 범위 (min - max)
    std::pair<float, float> FadeInPosFactor     = {0.0f, 0.0f};                 // 페이드인 위치 비율 (0 ~ 1)
    std::pair<float, float> FadeOutPosFactor    = {1.0f, 1.0f};                 // 페이드아웃 위치 비율 (0 ~ 1)

    std::unordered_map<int, std::vector<std::string>> WeaponQTETrackData;       // 무기 ID 별 QTE 트랙 파일 경로
    REFLECT_FIELDS_END(QTESystem)

    // QTE 편집기
    QTEEditor&  GetEditor();
};
