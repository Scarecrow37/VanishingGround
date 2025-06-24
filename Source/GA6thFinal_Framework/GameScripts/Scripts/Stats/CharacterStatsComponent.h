#pragma once
#include "UmFramework.h"

struct CharacterStats;
class CharacterStatsComponent abstract : public Component
{
    USING_PROPERTY(CharacterStatsComponent)
public:
    REFLECT_PROPERTY()

public:
    CharacterStatsComponent();
    virtual ~CharacterStatsComponent();

protected:
    // 이 함수를 override 해서 CharacterStats를 동적할당해 반환해야 합니다.
    virtual CharacterStats* NewCharacterStats() = 0;

    // Reset override시 CharacterStats::Reset() 호출 필요.
    virtual void Reset() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string StatsData{};
    REFLECT_FIELDS_END(CharacterStatsComponent)

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    virtual void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() override;

private:
    std::unique_ptr<CharacterStats> _stats;
};
