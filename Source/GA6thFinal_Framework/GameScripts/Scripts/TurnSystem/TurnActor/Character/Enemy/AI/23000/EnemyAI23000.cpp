#include "pchScripts.h"
#include "EnemyAI23000.h"

EnemyAI23000::EnemyAI23000()
{
    {
        ActionNode* node = new ActionNode();
        _action21101     = node;
        node->Action     = []() {
            // AI 행동 1
            UmLogger.Message(LogLevel::LEVEL_INFO, (const char*)u8"찢어발기기!!!!");
        };
    }
    {
        ActionNode* node = new ActionNode();
        _action21102     = node;
        node->Action     = []() {
            // AI 행동 2
            UmLogger.Message(LogLevel::LEVEL_INFO, (const char*)u8"기습!!!!!");
        };
    }
    {
        ActionNode* node = new ActionNode();
        _action21103     = node;
        node->Action     = []() {
            // AI 행동 3
            UmLogger.Message(LogLevel::LEVEL_INFO, (const char*)u8"확인 사살!!!!");
        };
    }
    {
        ConditionNode* node = new ConditionNode();
        _isBleeding         = node;
        node->Condition     = []() {
            int i = Random::Range(0, 1);
            return i == 0; // 조건이 참일 때
        };
        node->TrueNode  = _action21103; // 조건이 참일 때 실행할 노드
        node->FalseNode = _action21102; // 조건이 거짓일 때 실행할 노드
    }
    SetCurrentNode(_action21101); // 시작 노드(Entry)를 설정
    _action21101->NextNode = _isBleeding; // 행동 1 다음에 조건 검사 노드 연결
    _isBleeding->TrueNode  = _action21102; // 조건이 참일 때 행동 2 실행
    _isBleeding->FalseNode = _action21103; // 조건이 거짓일 때 행동 3 실행

    _action21103->NextNode = _action21101; // 행동 3 다음에 종료 노드 연결
    _action21102->NextNode = _isBleeding;  // 행동 2 다음에 조건 검사 노드
}

EnemyAI23000::~EnemyAI23000() 
{
    delete _action21101;
    delete _action21102;
    delete _action21103;
    delete _isBleeding;
}
