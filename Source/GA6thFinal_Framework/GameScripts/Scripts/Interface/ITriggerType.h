#pragma once

class CharacterBase;

class ITriggerType
{
private:
    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnCombatStart(CharacterBase* source) = 0;

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnRoundStart(CharacterBase* source) = 0;

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnRoundEnd(CharacterBase* source) = 0;

    /// <summary>존재하는 각 CharacterBase의 턴이 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnEachTurnStart(CharacterBase* source, CharacterBase* destination) = 0;

    /// <summary>턴이 시작될 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTurnStart(CharacterBase* source) = 0;

    /// <summary>턴이 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTurnEnd(CharacterBase* source) = 0;

    /// <summary>객체가 Hit 당했을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnHit(CharacterBase* source) = 0;

    /// <summary>객체가 사망했을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnDead(CharacterBase* source) = 0;

    /// <summary>객체가 대상을 처치 시 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    /// <param name="destination">처치된 대상</param>
    virtual void OnKill(CharacterBase* source, CharacterBase* destination) = 0;

    /// <summary>객체가 토큰을 얻었을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTokenAdded(CharacterBase* source, int tokenID) = 0;

    /// <summary>객체가 토큰이 제거되었을 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnTokenRemoved(CharacterBase* source, int tokenID) = 0;

    /// <summary>객체의 QTE가 시작할 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnQTEStart(CharacterBase* source) = 0;
    
    /// <summary>객체의 QTE가 끝날 때 호출됩니다.</summary>
    /// <param name="source">호출한 CharacterBase 객체입니다.</param>
    virtual void OnQTEEnd(CharacterBase* source)   = 0;
};