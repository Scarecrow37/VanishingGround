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
    inline static QTESystem* _staticInstance = nullptr;
public:
    QTESystem();
    virtual ~QTESystem();
    inline static QTESystem* GetInstance() { return _staticInstance; }

private:
    void Reset() override;
    void Awake() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    bool        AddMappingTrackToWeaponID(int weaponID, const File::Path& path = File::NULL_PATH);
    bool        RemoveMappingTrackToWeaponID(int weaponID, int index = -1);
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
