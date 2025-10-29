#pragma once
#include "Base/TurnModeStateBase.h"

class RoundInfoUIManager;

/*
* 라운드 시작 상태입니다.
* 라운드 시작 연출을 실행합니다.
* 라운드의 증가, TurnList 생성합니다.
*/
class RoundStartPhase : public TurnModeStateBase
{
public:
    RoundStartPhase();

    /// <summary>
    /// 페이즈 종료 여부를 반환합니다.
    /// </summary>
    /// <returns></returns>
    bool IsPhaseEnd() const { return _isPhaseEnd; }

protected:
    void OnAwake() override {};
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

    void NotifyRoundStart();

private:
    bool _isPhaseEnd;
    std::weak_ptr<RoundInfoUIManager> _roundInfoUIManager;

};