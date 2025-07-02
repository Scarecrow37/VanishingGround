#pragma once

/// <summary>
/// Time System의 Invoke 함수를 사용하기 위해선 다음 인터페이스를 상속받아야 합니다.
/// </summary>
struct ITimeInvoker
{
    ITimeInvoker() = default;
    virtual ~ITimeInvoker() = default;
    virtual std::weak_ptr<ITimeInvoker> GetWeakInvoker() = 0;
};

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

        /*Invoke 대기열을 clear 합니다.*/
        static void CleanUpInvokeFuntions();
    };
private:
    /*엔진 Update를 제어하기 위한 함수. 매프레임 호출해야함.*/
    void TimeSystemUpdate();

    /*엔진 Fixed Update를 제어하기 위한 함수. true를 반환하면 Fixed Update를 호출하면 됨.*/
    bool TimeSystemFixedUpdate();

    /*딜레이 함수들을 업데이트합니다. TimeScale 영향 받습니다.*/
    void UpdateInvokeFunctions();

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

    /// <summary>
    /// <para> 딜레이 시간 이후 함수를 호출해줍니다. </para>
    /// <para> weakPtr을 통해 유효성 확인이 가능한 함수만 등록 가능합니다. </para>
    /// </summary>
    /// <param name="object :">유효성 검사용 ITimeInvoker 객체</param>
    /// <param name="delay :">지연 시간</param>
    /// <param name="func :">호출할 함수</param>
    void Invoke(ITimeInvoker* object, float delay, const std::function<void()>& func) 
    {    
        _safeInvokeFunctions.emplace_back(object->GetWeakInvoker(), delay, func, 0.f);
    }

    /// <summary>
    /// <para> 딜레이 시간 이후 함수를 호출해줍니다. </para>
    /// <para> weakPtr을 통해 유효성 확인이 가능한 함수만 등록 가능합니다. </para>
    /// </summary>
    /// <param name="object :">유효성 검사용 ITimeInvoker 객체</param>
    /// <param name="delay :">지연 시간</param>
    /// <param name="func :">호출할 함수</param>
    void Invoke(ITimeInvoker& object, float delay, const std::function<void()>& func)
    { 
        Invoke(&object, delay, func);
    }

    /// <summary>
    /// <para> 딜레이 시간 이후 함수를 호출해줍니다. </para>
    /// <para> 주의 : 댕글링 접근 가능성이 있는 함수는 꼭 ITimeInvoker를 같이 넘겨야 합니다. </para>
    /// </summary>
    /// <param name="delay"></param>
    /// <param name="func"></param>
    void Invoke(float delay, const std::function<void()>& func) 
    { 
        _unsafeInvokeFunctions.emplace_back(delay, func, 0.f);
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

    //weakPtr, 지연시간, 함수, 현재시간
    std::vector<std::tuple<std::weak_ptr<ITimeInvoker>, float, std::function<void()>, float>> _safeInvokeFunctions;

    //지연시간, 함수, 현재시간
    std::vector<std::tuple<float, std::function<void()>, float>> _unsafeInvokeFunctions;
};
