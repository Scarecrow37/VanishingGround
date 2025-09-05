#pragma once

class QTEEditor;
namespace QTE
{
    class Track;
} 

class QTESystem : public Component
{
    USING_PROPERTY(QTESystem)
    inline static QTESystem* _staticInstance = nullptr;
public:
    QTESystem();
    ~QTESystem() override;
    inline static QTESystem* GetInstance() { return _staticInstance; }

private:
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    /// <summary>
    /// QTE 트랙을 통해 QTE를 시작합니다.
    /// </summary>
    /// <param name="track">시작할 QTE::Track 객체에 대한 포인터입니다.</param>
    void StartQTE(QTE::Track* track);

public:

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

private:
    QTE::Track* _qteTrack           = nullptr;      // QTE 트랙
    float       _delayTimer         = 0.0f;         // 딜레이 타이머
    float       _qteTimer           = 0.0f;         // 미리보기 타이머
    bool        _isQTEPlaying       = false;        // 미리보기 재생 여부

    REFLECT_FIELDS_BEGIN(Component)
    float QTESpeedScale     = 1.0f; // QTE 속도 배율
    float DelayFromQTEStart = 0.0f; // QTE 시작 대기 시간
    float JudgePosFactor = 0.8f; // 퍼펙트 노트 위치 비율 (0 ~ 1)
    std::pair<float, float> PerfectJudgeRange = {-0.05f, 0.05f}; // 퍼펙트 판정 범위 (min - max)
    std::pair<float, float> NormalJudgeRange  = {-0.1f, 0.1f}; // 노멀 판정 범위 (min - max)
    std::pair<float, float> FadeInPosFactor = {0.0f, 0.1f}; 
    std::pair<float, float> FadeOutPosFactor = {-0.1f, 0.1f}; 
    REFLECT_FIELDS_END(QTESystem)
    
    // QTE 편집기
    QTEEditor&  GetEditor();
};
