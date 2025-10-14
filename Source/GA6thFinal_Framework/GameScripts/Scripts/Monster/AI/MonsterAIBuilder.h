#pragma once
#include "Monster/AI/MonsterAI.h"

class Enemy;

namespace Monster
{
    class AIBuilder
    {
    public:
        static void Build();

    private:
        static void BuildAIController23000(std::weak_ptr<Enemy> owner, AIController& controller);
        static void BuildAIController23001(std::weak_ptr<Enemy> owner, AIController& controller);
        static void BuildAIController23010(std::weak_ptr<Enemy> owner, AIController& controller);
        static void BuildAIController23011(std::weak_ptr<Enemy> owner, AIController& controller);
        static void BuildAIController23020(std::weak_ptr<Enemy> owner, AIController& controller);
        static void BuildAIController23021(std::weak_ptr<Enemy> owner, AIController& controller);
        static void BuildAIController23022(std::weak_ptr<Enemy> owner, AIController& controller);
    };
}
