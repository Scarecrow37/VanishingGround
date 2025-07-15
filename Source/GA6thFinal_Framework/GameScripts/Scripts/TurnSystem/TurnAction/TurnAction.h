#pragma once
#include <Interface/ITriggerType.h>

class TurnAction abstract : public ITriggerType
{
    friend class TurnMode;
public:
    TurnAction() = default;
    ~TurnAction() override 
    { 
        SetDestroy();
    }

    /// <summary>
    /// 이 액션을 라이프 사이클에서 제외합니다.
    /// </summary>
    void SetDestroy()
    {
        if (_isDestroy)
        {
            *_isDestroy = true;
        }
    }

private:
    bool* _isDestroy = nullptr;

};