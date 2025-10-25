#pragma once
#include "QTE/Result/QTEResult.h"

namespace QTE
{
    struct OverallResult;
    class Callback
    {
    public:
        using Handle = int;
        Callback(std::weak_ptr<Component> weakOwner) 
            : Owner(weakOwner) {}
        Callback()  = default;
        ~Callback() = default;

        inline bool Invalid() { return Owner.expired(); }

        std::weak_ptr<Component> Owner;

        std::function<void()>                           OnFadeInStart   = nullptr;
        std::function<void()>                           OnFadeInFinish  = nullptr;
        std::function<void(const QTE::OverallResult&)>  OnFadeOutStart  = nullptr;
        std::function<void(const QTE::OverallResult&)>  OnFadeOutFinish = nullptr;
        std::function<void()>                           OnButtonPressed = nullptr;
        std::function<void(UINT, QTE::ResultType)>      OnNotePressed   = nullptr;
    };

    enum PlayState
    {
        STATE_WAITING,
        STATE_FADE_IN,
        STATE_PLAYING,
        STATE_FADE_OUT,
    };

    namespace Math
    {
        inline float Lerp(float a, float b, float t)
        {
            return a + (b - a) * t;
        }

        // QTE용 X가중치를 구합니다. 클램프는 하지 않습니다.
        // - deltaTime: 현재 트랙 시간과 노트 시간의차이
        // - speedScale: 속도 보정(트랙/게임 속도 배율)
        // - timeToPerfect: startX -> perfectX 까지 실제로 걸려야 하는 기준 시간 (0 초과여야함)
        // - return: 0 = 시작 지점, 1 = 퍼펙트 지점
        inline float CalculateNotePosXFactor(const float deltaTime, const float speedScale, const float timeToPerfect)
        {
            // 0 or 음수 나누기에 대한 예외 처리
            if (timeToPerfect <= 0.001f)
            {
                return 1.0f;
            }
            // 노트가 언제 생성됐는지 대비 현재 QTE 시간의 상대적 위치 계산
            const float scaledDelta = deltaTime * speedScale;
            const float factor      = 1.0f - (scaledDelta / timeToPerfect);
            return factor;
        }

        // QTE X 위치를 구합니다.
        inline float CalculateNotePosX(const float factorX, const float startX, const float endX)
        {
            return Lerp(startX, endX, factorX);
        }
    }
}
