#pragma once
#include "Base/GetTurnModeBaseC.h"

/*
 * 라운드 종료 연출이 종료됬는지 확인합니다.
 */
class CheckRoundEndExit : public GetTurnModeBaseC
{
public:
    CheckRoundEndExit();
    virtual ~CheckRoundEndExit();

    void OnAwake() override;
    void OnStart() override;
    bool Evaluate() override;
};