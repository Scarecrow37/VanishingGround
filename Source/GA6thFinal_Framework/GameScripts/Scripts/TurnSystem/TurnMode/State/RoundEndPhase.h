#pragma once
#include "Base/GetTurnModeBaseS.h"

/*
* 라운드 종료 페이즈용 State 입니다.
* 라운드 종료 연출을 실행해야합니다.
*/
class RoundEndPhase : public GetTurnModeBaseS
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

private:
    bool _isPhaseEnd = false;

};