#include "pchScripts.h"
#include "MonsterAI.h"

namespace Monster
{
    AIController::ActionNode::ActionNode(std::string_view label, std::string_view nextNode,
                                    const std::initializer_list<ActionData>& actions)
        : Node(label), _nextNode(nextNode)
    {
        for (const auto& action : actions)
        {
            _totalWeight += action.Weight;
            ActionData transAction = action; // 복사 생성자를 사용하여 액션 데이터를 복사
            transAction.Weight     = _totalWeight;
            _actionList.push_back(transAction);
        }
    }

    const std::string& AIController::ActionNode::NextNode() const
    {
        if (false == _nextNode.empty())
        {
            return _nextNode;
        }
        return _label;
    }

    void AIController::ActionNode::Refresh()
    {
        // 가중치에 따라 액션을 선택
        float randValue = Random::Range(0.0f, _totalWeight);
        _selectedIndex  = 0;
        for (size_t i = 0; i < _actionList.size(); ++i)
        {
            if (randValue <= _actionList[i].Weight)
            {
                _selectedIndex = i;
                break;
            }
        }
    }

    bool AIController::ActionNode::IsActionNode() const
    {
        return true;
    }

    int AIController::ActionNode::GetActionID() const
    {
        if (_selectedIndex < _actionList.size())
        {
            return _actionList[_selectedIndex].ActionID;
        }
        return -1;
    }

    AIController::ConditionNode::ConditionNode(std::string_view label, std::string_view trueNode, std::string_view falseNode,
                                          std::function<bool()> condition)
        : Node(label), _trueNode(trueNode), _falseNode(falseNode), _condition(condition)
    {
    }

    const std::string& AIController::ConditionNode::NextNode() const
    {
        if (true == _selectedBool)
        {
            if (false == _trueNode.empty())
            {
                return _trueNode;
            }
        }
        else
        {
            if (false == _falseNode.empty())
            {
                return _falseNode;
            }
        }
        return _label;
    }

    void AIController::ConditionNode::Refresh()
    {
        _selectedBool = _condition ? _condition() : false;
    }

    bool AIController::ConditionNode::IsActionNode() const
    {
        return false;
    }

    void AIController::Clear()
    {
        for (auto& [id, node] : _nodeTable)
        {
            delete node;
        }
        _nodeTable.clear();
        _currNode = nullptr;

        _transitionCount = 0;
    }

    void AIController::PushActionNode(std::string_view label, std::string_view nextNode, int actionID)
    {
        PushActionNode(label, nextNode, {ActionData(1.0f, actionID)}); // 가중치 1.0f로 단일 액션 추가
    }

    void AIController::PushActionNode(std::string_view label, std::string_view nextNode,
                                 std::initializer_list<ActionData> actions)
    {
        ActionNode* node         = new ActionNode(label, nextNode, actions);
        _nodeTable[label.data()] = node;
    }

    void AIController::PushConditionNode(std::string_view label, std::string_view trueNode, std::string_view falseNode,
                                         std::function<bool()> condition)
    {
        ConditionNode* node      = new ConditionNode(label, trueNode, falseNode, condition);
        _nodeTable[label.data()] = node;
    }

    void AIController::SetCurrentNode(std::string_view label)
    {
        const auto node = GetNode(label);
        if (nullptr != node)
        {
            _currNode = node;
        }
    }

    void AIController::Refresh()
    {
        if (nullptr != _currNode)
        {
            _currNode->Refresh();
            bool isActionNode = _currNode->IsActionNode();
            if (false == isActionNode)
            { // 조건노드는 다음 노드로 전환
                Transition();
            }
        }
    }

    void AIController::Transition()
    {
        if (nullptr != _currNode)
        {
            const auto& nextStr = _currNode->NextNode();
            const auto  nextptr = GetNode(nextStr);
            if (nextptr)
            {
                _currNode         = nextptr;
                bool isActionNode = _currNode->IsActionNode();
                ++_transitionCount;
            }
        }
    }

    int AIController::GetCurrentActionID() const
    {
        if (nullptr != _currNode)
        {
            return _currNode->GetActionID();
        }
        return -1;
    }

    AIController::Node* AIController::GetNode(std::string_view label) const
    {
        auto it = _nodeTable.find(label.data());
        if (it != _nodeTable.end())
        {
            return it->second;
        }
        return nullptr;
    }

}