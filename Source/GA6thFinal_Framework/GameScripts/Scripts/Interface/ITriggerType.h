#pragma once

class CharacterBase;

class ITriggerType
{
public:
    ITriggerType() = default;
    virtual ~ITriggerType() = default;

public:
    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    virtual void OnCombatStart() = 0;

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    virtual void OnRoundStart() = 0;

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    virtual void OnRoundEnd() = 0;

    /// <summary>존재하는 각 CharacterBase의 턴이 시작될 때 호출됩니다.</summary>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnEachTurnStart(CharacterBase* destination) = 0;

    /// <summary>턴이 시작될 때 호출됩니다.</summary>
    virtual void OnTurnStart() = 0;

    /// <summary>턴이 끝날 때 호출됩니다.</summary>
    virtual void OnTurnEnd() = 0;

    /// <summary>객체가 Hit 당했을 때 호출됩니다.</summary>    
    virtual void OnHit() = 0;

    /// <summary>객체가 대상을 처치 시 호출됩니다.</summary> 
    /// <param name="destination">처치된 대상</param>
    virtual void OnKill(CharacterBase* destination) = 0;

    /// <summary>객체가 토큰을 얻었을 때 호출됩니다.</summary> 
    virtual void OnTokenAdded(int tokenID) = 0;

    /// <summary>객체가 토큰이 제거되었을 때 호출됩니다.</summary>  
    virtual void OnTokenRemoved(int tokenID) = 0;

    /// <summary>객체가 토큰이 제거되었을 때 호출됩니다.</summary>
    virtual void OnTokenEnter(int tokenID) = 0;

    /// <summary>객체가 토큰이 제거되었을 때 호출됩니다.</summary>
    virtual void OnTokenExit(int tokenID) = 0;

    /// <summary>객체의 QTE가 시작할 때 호출됩니다.</summary>
    virtual void OnQTEStart() = 0;

    /// <summary>객체의 QTE가 끝날 때 호출됩니다.</summary>
    virtual void OnQTEEnd() = 0;

};
