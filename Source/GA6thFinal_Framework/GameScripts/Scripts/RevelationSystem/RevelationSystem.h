#pragma once
#include "RevelationAction/Base/RevelationActionBase.h"

//RevelationActionBase를 상속받은 클래스를 팩토리에 등록할때 사용합니다.
#define REGISTER_REVELATION_CLASS(REVELATION) REGISTER_CLASS(RevelationSystem, REVELATION)

class RevelationSystem : public Component, public FactoryConstructor<RevelationActionBase>
{
    USING_PROPERTY(RevelationSystem)      
public:
    static RevelationSystem* GetInstance() { return static_instance; }

public:
    RevelationSystem();
    ~RevelationSystem() override;

public:
    template<typename T>
    RevelationActionBase* GetAction()
    {
        static_assert(std::is_base_of_v<RevelationActionBase, T>("T must be derived from RevelationActionBase."));
        return GetActionToName(typeid(T).name());
    }

    RevelationActionBase* GetActionToName(std::string_view typeidName) 
    { 
        const char* key = typeidName.data();
        return _actions[key].get(); 
    }

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::pair<std::string, std::string>> RevelationActionDatas;
    REFLECT_FIELDS_END(RevelationSystem)

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

    void Reset() override;

private:
    inline static RevelationSystem* static_instance = nullptr;

private:
    std::unordered_map<std::string, std::unique_ptr<RevelationActionBase>> _actions;

};
