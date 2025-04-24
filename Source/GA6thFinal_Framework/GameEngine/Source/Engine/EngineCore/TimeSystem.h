#pragma once

class ETimeSystem
{
    friend class EngineCores;
private:
    ETimeSystem();
    ~ETimeSystem();

public:
    //엔진 접근용 네임스페이스
    struct Engine
    {
        /*엔진 Update를 제어하기 위한 함수. 매프레임 호출해야함. */
        static void TimeSystemUpdate();

        /*엔진 Fixed Update를 제어하기 위한 함수. true를 반환하면 Fixed Update를 호출하면 됨.*/
        static bool TimeSystemFixedUpdate();
    };
private:
    /*엔진 Update를 제어하기 위한 함수. 매프레임 호출해야함.*/
    void TimeSystemUpdate();

    /*엔진 Fixed Update를 제어하기 위한 함수. true를 반환하면 Fixed Update를 호출하면 됨.*/
    bool TimeSystemFixedUpdate();

public:
    /*시간이 경과하는 속도를 제어합니다. */
    double TimeScale = 1.0;

    /*Fixed Update가 호출되는 주기시간을 제어합니다. 기본값 : 0.02f (1초에 50번)*/
    double FixedTimeStep = 0.02;

    /*deltaTime의 최대값을 제어합니다. 반드시 FixedTimeStep 이상이어야 합니다. 기본값 : 0.333333*/
    double MaximumDeltaTime = 0.33333333333333333;

public:
    /*델타 타임. 단위 : 초 */
    inline float DeltaTime() const
    {
        return (float)_deltaTime;
    }

    /*Time Scale 영향 안받는 DeltaTime*/
    inline float UnscaledDeltaTime() const
    {
        return (float)_unscaledDeltaTime;
    }

    /*Fixed DeltaTime 주기*/
    inline float FixedDeltaTime() const
    {
        return (float)_fixedDeltaTime;
    }

    /*Time Scale 영향 받는 실제 주기*/
    inline float FixedUnscaledDeltaTime() const
    {
        return (float)_fixedUnscaledDeltaTime;
    }

    /*게임이 시작된 이후의 총 프레임 수.*/
    inline unsigned long long FrameCount() const
    {
        return _frameCount;
    }

    /*프로그램 시작 후 경과한 실제 시간을 초 단위로 반환합니다.*/
    inline float RealtimeSinceStartup() const
    {
        return float(_realtimeSinceStartup) / float(_frequency.QuadPart);
    }

    /*프로그램 시작 후 경과한 실제 시간을 초 단위로 반환합니다. (정밀도 double)*/
    inline double RealtimeSinceStartupAsDouble() const
    {
        return double(_realtimeSinceStartup) / double(_frequency.QuadPart);
    }

    /*프로그램 시작 후 경과한 시간을 초 단위로 반환합니다. (TimeScale 영향 받습니다.)*/
    inline float Time() const
    {
        return float(_time) / float(_frequency.QuadPart);
    }

    /*프로그램 시작 후 경과한 시간을 초 단위로 반환합니다. (정밀도 double)*/
    inline double TimeAsDouble() const
    {
        return double(_time) / double(_frequency.QuadPart);
    }

private:
    LARGE_INTEGER _previousTime{};
    LARGE_INTEGER _currentTime{};
    LARGE_INTEGER _frequency{};

    LONGLONG	  _time{};
    LONGLONG	  _realtimeSinceStartup{};

    double		  _deltaTime{};
    double		  _unscaledDeltaTime{};

    double		  _elapsedFixedTime{};
    double		  _fixedDeltaTime{};
    double		  _fixedUnscaledDeltaTime{};

    unsigned long long _frameCount{};
};
