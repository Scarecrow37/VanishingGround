#include "pchScripts.h"
#include "EnemyAI.h"

EnemyAI::Node* EnemyAI::ActionNode::Process()
{
    if (Action)
    {
        Action();
    }
    return NextNode;
}

EnemyAI::Node* EnemyAI::ConditionNode::Process()
{
    if (Condition)
    {
        if (Condition())
        {
            if (TrueNode)
            {
                return TrueNode->Process();
            }
        }
        else
        {
            if (FalseNode)
            {
                return FalseNode->Process();
            }
        }
    }
    return nullptr;
}

void EnemyAI::Process(bool transition)
{
    if (nullptr != _currNode)
    {
        Node* next = _currNode;
        next = _currNode->Process();
        if (true == transition)
        {
            _currNode = next;
        }
    }
}

void EnemyAI::SetCurrentNode(Node* node)
{
    _currNode = node;
}
