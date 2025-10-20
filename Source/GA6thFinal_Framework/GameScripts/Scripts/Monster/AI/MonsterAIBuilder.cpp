#include "pchScripts.h"
#include "MonsterAIBuilder.h"
#include "MonsterAIFactory.h"

#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

#include "Token/TokenInventory.h"
#include "Token/Object/Bleed/BleedToken.h"

namespace Monster
{
    // Monster A
    void BuildAIModel210300(std::weak_ptr<Enemy> owner, AIModel& controller) 
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210200);  // Action 210200
        controller.PushActionNode("#3", "#2", 210201);  // Action 210201
        controller.PushActionNode("#4", "#1", 210202);  // Action 210202
        controller.PushConditionNode("#2", "#3", "#4",  // 조건 노드
                []() -> bool {
                if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
                {
                    // 플레이어가 출혈 토큰을 가지고 있는지 확인
                    if (Player* player = turnMode->GetPlayer()) 
                    {
                        auto& tokenInventory = player->GetTokenInventory();
                        return tokenInventory.HasTokenFromID(TokenObject::Bleed::ID);
                    }
                }
                return false;
        });

        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }
    void BuildAIModel210301(std::weak_ptr<Enemy> owner, AIModel& controller) 
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210204);                              // Action 210204
        controller.PushActionNode("#2", "#3", {{50.0f, 210203}, {50.0f, 210200}});  // Action 210203, 210200
        controller.PushActionNode("#3", "#4", 210202);                              // Action 22002
        controller.PushActionNode("#4", "#1", {{50.0f, 210200}, {50.0f, 210202}});  // Action 210200, 210202
        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }
    void BuildAIModel210302(std::weak_ptr<Enemy> owner, AIModel& controller)
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210204);                             // Action 210204
        controller.PushActionNode("#2", "#3", 210202);                             // Action 210203, 210200
        controller.PushActionNode("#3", "#1", 210203);                             // Action 210203, 210200
        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }

    // Monster B
    void BuildAIModel210310(std::weak_ptr<Enemy> owner, AIModel& controller) 
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", {{50.0f, 210210}, {50.0f, 210211}});                  // Action 210210, 210211
        controller.PushActionNode("#2", "#3", {{50.0f, 210210}, {50.0f, 210211}});                  // Action 210210, 210211
        controller.PushActionNode("#3", "#4", 210213);                                              // Action 22013
        controller.PushActionNode("#4", "#3", {{15.0f, 210210}, {15.0f, 210211}, {70.0f, 210212}}); // Action 210210, 210211

        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }
    void BuildAIModel210311(std::weak_ptr<Enemy> owner, AIModel& controller) 
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210214);                                              // Action 210214
        controller.PushActionNode("#2", "#3", {{25.0f, 210210}, {25.0f, 210211}, {50.0f, 210214}}); // Action 210210, 210211
        controller.PushActionNode("#3", "#4", 210213);                                              // Action 22013
        controller.PushActionNode("#2", "#3", {{70.0f, 210212}, {30.0f, 210211}});                  // Action 210212, 210211
        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }
    void BuildAIModel210312(std::weak_ptr<Enemy> owner, AIModel& controller)
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210214); // Action 210214
        controller.PushActionNode("#2", "#3", {{25.0f, 210210}, {25.0f, 210211}, {50.0f, 210214}}); // Action 210210, 210211
        controller.PushActionNode("#3", "#4", 210214);                                  // Action 22013
        controller.PushActionNode("#4", "#1", 210213);                                  // Action 22013
        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }

    // Monster C
    void BuildAIModel210320(std::weak_ptr<Enemy> owner, AIModel& controller) 
    {
        controller.Clear();
    }
    void BuildAIModel210321(std::weak_ptr<Enemy> owner, AIModel& controller)
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210220);
        controller.PushConditionNode("#2", "#4", "#3",
            [owner]() -> bool {
            if (auto enemy = owner.lock())
            {
                float hpRatio = (float)enemy->HP / (float)enemy->MaxHP;
                return hpRatio <= 0.5f; // HP가 50% 이하일 때 true 반환
            }
            return false;
        });
        controller.PushActionNode("#3", "#5", 210122);
        controller.PushActionNode("#4", "#5", {{50.0f, 210122}, {50.0f, 210123}});
        controller.PushActionNode("#5", "#2", {{50.0f, 210120}, {50.0f, 210121}});

        // Entry 노드 설정
        controller.SetCurrentNode("#1");
    }
    void BuildAIModel210322(std::weak_ptr<Enemy> owner, AIModel& controller)
    {
        controller.Clear();
        controller.PushActionNode("#1", "#2", 210220);
        controller.PushActionNode("#2", "#3", 210221);
        controller.PushActionNode("#3", "#2", 210220);
    }

    #define REGISTER_AI(ID)                                                     \
    namespace AIBuilder##ID                                                     \
    {                                                                           \
        bool registered = AIFactory::RegisterAIBuilder(ID, &BuildAIModel##ID);  \
    }

    REGISTER_AI(210300)
    REGISTER_AI(210301)
    REGISTER_AI(210302)

    REGISTER_AI(210310)
    REGISTER_AI(210311)
    REGISTER_AI(210312)

    REGISTER_AI(210320)
    REGISTER_AI(210321)
    REGISTER_AI(210322)
}
