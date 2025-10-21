#include "pchScripts.h"
#include "MonsterAIModel.h"

namespace Monster
{
    AIModel::ActionNode::ActionNode(std::string_view label, std::string_view nextNode,
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

    const std::string& AIModel::ActionNode::NextNode() const
    {
        if (false == _nextNode.empty())
        {
            return _nextNode;
        }
        return _label;
    }

    void AIModel::ActionNode::Refresh()
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

    bool AIModel::ActionNode::IsActionNode() const
    {
        return true;
    }

    int AIModel::ActionNode::GetActionID() const
    {
        if (_selectedIndex < _actionList.size())
        {
            return _actionList[_selectedIndex].ActionID;
        }
        return -1;
    }

    AIModel::ConditionNode::ConditionNode(std::string_view label, std::string_view trueNode, std::string_view falseNode,
                                          std::function<bool()> condition)
        : Node(label), _trueNode(trueNode), _falseNode(falseNode), _condition(condition)
    {
    }

    const std::string& AIModel::ConditionNode::NextNode() const
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

    void AIModel::ConditionNode::Refresh()
    {
        _selectedBool = _condition ? _condition() : false;
    }

    bool AIModel::ConditionNode::IsActionNode() const
    {
        return false;
    }

    void AIModel::Clear()
    {
        for (auto& [id, node] : _nodeTable)
        {
            delete node;
        }
        _nodeTable.clear();
        _currNode = nullptr;

        _transitionCount = 0;
    }

    void AIModel::PushActionNode(std::string_view label, std::string_view nextNode, int actionID)
    {
        PushActionNode(label, nextNode, {ActionData(1.0f, actionID)}); // 가중치 1.0f로 단일 액션 추가
    }

    void AIModel::PushActionNode(std::string_view label, std::string_view nextNode,
                                 std::initializer_list<ActionData> actions)
    {
        ActionNode* node         = new ActionNode(label, nextNode, actions);
        _nodeTable[label.data()] = node;
    }

    void AIModel::PushConditionNode(std::string_view label, std::string_view trueNode, std::string_view falseNode,
                                         std::function<bool()> condition)
    {
        ConditionNode* node      = new ConditionNode(label, trueNode, falseNode, condition);
        _nodeTable[label.data()] = node;
    }

    void AIModel::SetCurrentNode(std::string_view label)
    {
        const auto node = GetNode(label);
        if (node)
        {
            _currNode = node;
        }
    }

    void AIModel::SetEntryNode(std::string_view label) 
    {
        const auto node = GetNode(label);
        if (node)
        {
            _entryNode = node;
            _currNode  = node;
        }
    }

    void AIModel::Refresh()
    {
        if (_currNode)
        {
            _currNode->Refresh();
            bool isActionNode = _currNode->IsActionNode();
            if (false == isActionNode)
            { // 조건노드는 다음 노드로 전환
                Transition();
            }
        }
    }

    void AIModel::Reset()
    {
        if (_entryNode)
        {
            _currNode = _entryNode;
            _transitionCount = 0;
        }
    }

    void AIModel::Transition()
    {
        if (_currNode)
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

    int AIModel::GetCurrentActionID() const
    {
        if (_currNode)
        {
            return _currNode->GetActionID();
        }
        return -1;
    }

    const char* AIModel::GetCurrentNodeLabel() const
    {
        if (_currNode)
        {
            return _currNode->GetLabel().c_str();
        }
        return nullptr;
    }

    size_t AIModel::GetNodeCount() const
    {
        return _nodeTable.size();
    }

    AIModel::Node* AIModel::GetNode(std::string_view label) const
    {
        auto it = _nodeTable.find(label.data());
        if (it != _nodeTable.end())
        {
            return it->second;
        }
        return nullptr;
    }

}