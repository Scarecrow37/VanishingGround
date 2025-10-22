#include "pchScripts.h"
#include "LastQTEResultCondition.h"
#include "QTE/System/QTESystem.h"

REGISTER_TURN_ACTION_CONDITION(LastQTEResultCondition)

REFLECT_FUNCTION(LastQTEResultCondition)

LastQTEResultCondition::LastQTEResultCondition()
{
    UpdateConditionInfo();
}

LastQTEResultCondition::~LastQTEResultCondition() = default;

bool LastQTEResultCondition::Evaluate()
{
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        const QTE::OverallResult& overall = system->GetQTEOverallResult();
        ResultType type    = ReflectFields->Type;
        switch (type)
        {
        case LastQTEResultCondition::ResultType::ALL_CRIT:
            return overall.CompareResult(QTE::QTE_RESULT_ALL_CRIT);
        case LastQTEResultCondition::ResultType::OVER_HIT:
            return overall.CompareResult(QTE::QTE_RESULT_OVER_HIT);
        case LastQTEResultCondition::ResultType::ALL_CRIT_FAIL:
            return false == overall.CompareResult(QTE::QTE_RESULT_ALL_CRIT);
        case LastQTEResultCondition::ResultType::OVER_HIT_FAIL:
            return false == overall.CompareResult(QTE::QTE_RESULT_OVER_HIT);
        default:
            return false;
        }
    }
    return false;
}

void LastQTEResultCondition::DrawImguiEditor() 
{
    static const std::array<std::string_view, 4> tooltips = 
    {
        (const char*)u8"치명적", 
        (const char*)u8"무결점",
        (const char*)u8"치명적 실패",
        (const char*)u8"무결점 실패"
    };

    ImGuiHelper::EnumCombo<ResultType>("QTE Type", ReflectFields->Type, [this](const std::pair<std::string_view, ResultType>& pair) 
    { 
        ReflectFields->Type = pair.second;
        UpdateConditionInfo();
    },
    &tooltips);
}

const std::string& LastQTEResultCondition::GetConditionInfo() const
{
    return conditionInfo;
}

void LastQTEResultCondition::DeserializedReflectEvent() 
{
    UpdateConditionInfo();
}

void LastQTEResultCondition::UpdateConditionInfo()
{
    using namespace u8_literals;

    conditionInfo = (const char*)u8"마지막 QTE 결과가 ";
    ResultType type = ReflectFields->Type;
    switch (type)
    {
    case LastQTEResultCondition::ResultType::ALL_CRIT:
        conditionInfo += u8"치명적"_c_str;
        break;
    case LastQTEResultCondition::ResultType::OVER_HIT:
        conditionInfo += u8"무결점"_c_str;
        break;
    case LastQTEResultCondition::ResultType::ALL_CRIT_FAIL:
        conditionInfo += u8"치명적 실패"_c_str;
        break;
    case LastQTEResultCondition::ResultType::OVER_HIT_FAIL:
        conditionInfo += u8"무결점 실패"_c_str;
        break;
    default:
        conditionInfo = STR_NULL;
        break;
    }
}