#pragma once
#include "UmFrameWork.h"
#include <Interface/ITriggerType.h>

//턴 라이프 사이클 사용을 위한 Base 클래스입니다.
class TurnAction abstract : public ReflectSerializer
{
    USING_PROPERTY(TurnAction)
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
    /*Action의 이름을 반환해야합니다.*/
    virtual std::string_view GetActionName() = 0;

    /*Action의 효과를 정의하는 내용을 반환해야합니다.*/
    virtual std::string_view GetActionInfo() = 0;

    /*편집을 위한 ImGui 함수를 구현해야합니다.*/
    virtual void ImGuiDrawActionEditor() = 0;

    /// <summary>전투가 시작될 때 호출됩니다.</summary>
    virtual void OnCombatStart() {}

    /// <summary>라운드가 시작될 때 호출됩니다.</summary>
    virtual void OnRoundStart() {}

    /// <summary>라운드가 끝날 때 호출됩니다.</summary>
    virtual void OnRoundEnd() {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 시작되면 호출됩니다..</summary>
    /// <param name="destination">턴이 시작된 대상</param>
    virtual void OnTurnStart(CharacterBase* destination) {}

    /// <summary>인자로 넘어온 캐릭터의 턴이 종료되면 호출됩니다.</summary>
    virtual void OnTurnEnd(CharacterBase* destination) {}

public:
    REFLECT_PROPERTY(Name)

    GETTER_ONLY(std::string_view, Name) { return GetActionName(); }
    // 계시 이름
    PROPERTY(Name)

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(TurnAction)

private:
    bool* _isDestroy = nullptr;

};