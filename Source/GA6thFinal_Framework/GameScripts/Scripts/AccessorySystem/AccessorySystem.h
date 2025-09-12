#pragma once
#include "UmFramework.h"
#include "Utility/SingletonHelper.h"
#include "AccessoryElement/AccessoryElement.h"

class AccessorySystem : public Component
{
    USING_PROPERTY(AccessorySystem)

public:
    AccessorySystem();
    ~AccessorySystem() override;

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::pair<std::string, std::string>> ElementTableData;
    REFLECT_FIELDS_END(AccessorySystem)

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

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

    void Reset() override;
    void Awake() override;

private:
    SingletonComponent<AccessorySystem> _singletonComponent{this};

    std::map<std::string, AccessoryElement> _elementTable;        // 이름-액세서리 테이블
    std::vector<AccessoryElement*>          _elementTableOrderID; // 아이디 순 정렬된 테이블

private:
    void ElementTableSerialized();
    void ElementTableDeserialized();

};
