#pragma once

class EnemyAI
{
public:
    struct Node
    {
        virtual Node* Process() = 0;
    };
    struct ActionNode : public Node
    {
        Node* NextNode = nullptr; // 다음 노드

        std::function<void()> Action; // 실행할 액션
       
        Node* Process() override;
    };
    struct ConditionNode : public Node
    {
        Node* TrueNode  = nullptr; // 조건이 참일 때 실행할 노드
        Node* FalseNode = nullptr; // 조건이 거짓일 때 실행할 노드

        std::function<bool()> Condition; // 조건 함수
        
        Node* Process() override;
    };

    /// <summary>
    /// 현재 노드를 처리합니다.
    /// </summary>
    /// <param name="transition">다음 노드로 넘어가는지에 대한 여부입니다.</param>
    void Process(bool transition = true);

protected:
    void SetCurrentNode(Node* node);

    Node* _currNode  = nullptr;
};
