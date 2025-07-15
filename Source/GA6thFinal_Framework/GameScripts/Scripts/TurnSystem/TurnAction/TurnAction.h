#pragma once
#include <Interface/ITriggerType.h>

//턴 라이프 사이클 사용을 위한 Base 클래스입니다.
class TurnAction abstract
{
    friend class TurnMode;
public:
    TurnAction() = default;
    virtual ~TurnAction()
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

public:
    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    virtual void OnCombatStart() = 0;

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    virtual void OnRoundStart() = 0;

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    virtual void OnRoundEnd() = 0;

    /// <summary>인자로 넘어온 캐릭터의 턴이 시작되면 호출됩니다..</summary>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnEachTurnStart(CharacterBase* destination) = 0;

    /// <summary>턴이 시작될 때 호출됩니다.</summary>
    virtual void OnTurnStart() = 0;

    /// <summary>턴이 끝날 때 호출됩니다.</summary>
    virtual void OnTurnEnd() = 0;

private:
    bool* _isDestroy = nullptr;

};