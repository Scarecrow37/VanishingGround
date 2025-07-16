#pragma once
#include "Base/TurnModeStateBase.h"

/*
* 라운드 종료 페이즈용 State 입니다.
* 라운드 종료 연출을 실행해야합니다.
*/
class RoundEndPhase : public TurnModeStateBase
{
public:
    RoundEndPhase();
    virtual ~RoundEndPhase() override;

    /// <summary>
    /// 페이즈 종료 여부를 반환합니다.
    /// </summary>
    /// <returns></returns>
    bool IsPhaseEnd() const { return _isPhaseEnd; }

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    /// <summary>
    /// 라운드에 존재하는 객체에게 OnRoundEnd를 호출해줍니다.
    /// </summary>
    void NotifyRoundEnd();

private:
    bool _isPhaseEnd = false;

};