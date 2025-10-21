#pragma once

namespace Monster
{
    class AIModel final
    {
    public:
        AIModel()  = default;
        ~AIModel() = default;

    public:
        struct ActionData
        {
            float Weight;   // 가중치
            int   ActionID; // 액션 ID
        };
        class Node
        {
        public:
            Node(std::string_view label) : _label(label) {}
            virtual ~Node() = default;

        public:
            virtual const std::string& NextNode() const     = 0;
            virtual void               Refresh()            = 0;
            virtual bool               IsActionNode() const = 0;
            virtual int                GetActionID() const { return -1; }
            inline const std::string&  GetLabel() const { return _label; }

        protected:
            std::string _label;
        };
        class ActionNode : public Node
        {
        public:
            ActionNode(std::string_view label, std::string_view nextNode,
                       const std::initializer_list<ActionData>& actions);
            virtual ~ActionNode() = default;

        private:
            const std::string& NextNode() const override;
            void               Refresh() override;
            bool               IsActionNode() const override;
            int                GetActionID() const override;

            std::string             _nextNode      = ""; // 다음 노드
            size_t                  _selectedIndex = 0;
            std::vector<ActionData> _actionList;         // 가중치와 액션 ID
            float                   _totalWeight = 0.0f; // 액션 가중치의 총합
        };
        class ConditionNode : public Node
        {
        public:
            ConditionNode(std::string_view label, std::string_view trueNode, std::string_view falseNode,
                          std::function<bool()> condition);
            virtual ~ConditionNode() = default;

        private:
            const std::string& NextNode() const override;
            void               Refresh() override;
            bool               IsActionNode() const override;

            std::string           _trueNode     = "";    // 조건이 참일 때 실행할 노드
            std::string           _falseNode    = "";    // 조건이 거짓일 때 실행할 노드
            bool                  _selectedBool = false; // 조건으로 선택된 여부
            std::function<bool()> _condition;            // 조건 함수
        };

    public:
        /// <summary>
        /// 모든 노드를 제거하고 초기화합니다.
        /// </summary>
        void Clear();

        /// <summary>
        /// 노드를 초기 상태로 재설정합니다.
        /// </summary>
        void Reset();

        /// <summary>
        /// 액션 노드를 추가합니다.
        /// </summary>
        /// <param name="label">노드를 식별할 label입니다.</param>
        /// <param name="nextNode">다음으로 전이할 노드 label입니다.</param>
        /// <param name="actionID">해당 노드의 액션 ID 값입니다.</param>
        void PushActionNode(std::string_view label, std::string_view nextNode, int actionID);

        /// <summary>
        /// 액션 노드를 추가합니다.
        /// </summary>
        /// <param name="label">노드를 식별할 label입니다.</param>
        /// <param name="nextNode">다음으로 전이할 노드 label입니다.</param>
        /// <param name="actions">해당 노드의 가중치, 액션 ID 값의 쌍을 들고있는 컨테이너입니다.</param>
        void PushActionNode(std::string_view label, std::string_view nextNode,
                            std::initializer_list<ActionData> actions);

        /// <summary>
        /// 조건 노드를 추가합니다.
        /// </summary>
        /// <param name="label">노드를 식별할 label입니다.</param>
        /// <param name="trueNode">참일 시 전이할 노드 label입니다.</param>
        /// <param name="falseNode">거짓일 시 전이할 노드 label입니다.</param>
        /// <param name="condition">조건을 확인할 함수입니다. 인자로 호출자의 weak_ptr을 제공합니다.</param>
        void PushConditionNode(std::string_view label, std::string_view trueNode, std::string_view falseNode,
                               std::function<bool()> condition);

        /// <summary>
        /// 현재 노드를 설정합니다.
        /// Entry 노드 또한 이 메서드를 초기에 호출하여 설정할 수 있습니다.
        /// </summary>
        /// <param name="label">설정할 노드 label입니다.</param>
        void SetCurrentNode(std::string_view label);

        /// <summary>
        /// 지정된 레이블로 엔트리 노드를 설정합니다.
        /// </summary>
        /// <param name="label">설정할 노드의 레이블.</param>
        void SetEntryNode(std::string_view label);

        /// <summary>
        /// 노드를 갱신합니다. 이는 행동을 결정하거나, 조건을 평가하는 등의 작업을 수행합니다.
        /// <para>액션 노드는 액션 리스트에서 랜덤하게 가르키는 액션을 갱신합니다.</para>
        /// <para>조건 노드는 조건을 평가하여 다음 노드를 결정합니다.</para>
        /// </summary>
        void Refresh();

        /// <summary>
        /// <para>다음 노드로 넘어갑니다.</para>
        /// <para>다음 노드가 없으면 현재 노드를 유지합니다.</para>
        /// </summary>
        void Transition();

        /// <summary>
        /// 현재 액션 ID를 반환합니다.
        /// </summary>
        /// <returns> 현재 액션의 ID 값입니다. 액션 ID가 존재하지 않는 경우는 -1을 반환합니다.</returns>
        int GetCurrentActionID() const;

        /// <summary>
        /// 현재 노드의 레이블을 반환합니다.
        /// </summary>
        /// <returns>현재 노드의 레이블값입니다. 현재 노드가 널인 경우 nullptr을 반환합니다.</returns>
        const char* GetCurrentNodeLabel() const;

        size_t GetNodeCount() const;

    private:
        Node* GetNode(std::string_view label) const;

    private:
        Node*                                   _currNode  = nullptr;
        Node*                                   _entryNode = nullptr;
        std::unordered_map<std::string, Node*>  _nodeTable;
        size_t                                  _transitionCount = 0; // 전이 횟수
    };
}

