#pragma once
#include "TurnAction.h"

///Turn Action 객체를 팩토리에 등록합니다.
#define REGISTER_TURN_ACTION(CLASS) REGISTER_CLASS(TurnActionFactory, CLASS)

class TurnActionFactory : public FactoryConstructor<TurnAction>
{
public:
    using FactoryType = std::unordered_map<std::string, std::function<TurnAction*()>>;
    /// <summary>
    /// 액션 이름으로 키를 사용하는 생성자 함수를 담은 map을 반환합니다.
    /// </summary>
    /// <returns></returns>
    inline static const FactoryType& GetActionFactory()
    { 
        static FactoryType factory;
        if (factory.empty() == true)
        {
            for (auto& [key, makeFunc] : GetInstanceConstructors())
            {
                std::unique_ptr<TurnAction> temp;
                temp.reset(makeFunc());
                const std::string& name = temp->GetActionName();
                if (factory.find(name) == factory.end())
                {
                    factory[name] = makeFunc;
                }
                else
                {
                    auto msg = std::format("{}{}", (const char*)u8"이미 존재하는 액션 이름입니다.", name);
                    UmLogger.Log(LogLevel::LEVEL_WARNING, msg);
                    factory.clear();
                    break;
                }
            }
        }
        return factory;
    }

    TurnActionFactory() = delete;
    ~TurnActionFactory() override = default;
};