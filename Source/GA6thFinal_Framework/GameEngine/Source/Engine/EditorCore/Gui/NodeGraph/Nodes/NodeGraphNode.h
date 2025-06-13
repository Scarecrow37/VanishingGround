#pragma once
#include "NodeGraphPin.h"

class EditorNodeGraph;

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;

    /*
    노드 그래프에 나타날 노드를 정의하는 클래스입니다.
    이 클래스를 상속해 Draw를 구현하여 노드의 내용을 그립니다.
    */
    class Node
    {
        using PinIDTable = std::unordered_map<UINT64, Pin*>;
        using PinLabelTable = std::unordered_map<std::string, Pin*>;
    public:
        Node();
        virtual ~Node();

    public:
        virtual void Draw() = 0;
        virtual void Do()   = 0;

        virtual void OnCreate() {};
        virtual void OnNodePopup() {};
        virtual void OnPinPopup(UINT64 pinID) {};

    public:
        /// <summary>
        /// ID를 통해 핀을 찾습니다.
        /// </summary>
        /// <param name="id">핀의 ID값</param>
        /// <returns>핀을 찾는데 성공하면 해당 pin의 포인터, 찾지 못하면 null포인터를 반환합니다.</returns>
        Pin* FindPin(UINT64 id);

        /// <summary>
        /// 이름을 통해 핀을 찾습니다. label값 중복이 존재하는 경우 원하는 동작을 보장하지 않습니다.
        /// </summary>
        /// <param name="label">핀의 이름</param>
        /// <returns>핀을 찾는데 성공하면 해당 pin의 포인터, 찾지 못하면 null포인터를 반환합니다.</returns>
        Pin* FindPin(const char* label);

        /// <summary>
        /// 핀을 추가합니다.
        /// </summary>
        /// <param name="label">핀의 이름</param>
        /// <param name="type">핀의 식별 값(서로 간에 이을 수 있는 노드인지 확인하는 데 사용합니다.)</param>
        /// <param name="kind">핀의 유형(Input, Output)</param>
        /// <returns>추가한 핀의 포인터를 반환합니다.</returns>
        Pin* AddPin(const char* label, const char* type, ed::PinKind kind);

        /// <summary>
        /// ID값에 대응하는 핀을 제거합니다.
        /// </summary>
        /// <param name="id">핀의 ID값</param>
        /// <returns>삭제에 성공하면 true, 실패하면 false.</returns>
        bool RemovePin(UINT64 id);

        /// <summary>
        /// Label값에 대응하는 핀을 제거합니다. label값 중복이 존재하는 경우 원하는 동작을 보장하지 않습니다.
        /// </summary>
        /// <param name="label">핀의 이름</param>
        /// <returns>삭제에 성공하면 true, 실패하면 false.</returns>
        bool RemovePin(const char* label);

        bool RemovePin(Pin* pin);

        /// <summary>
        /// 노드의 포지션을 설정합니다.
        /// </summary>
        /// <param name="pos">설정할 포지션입니다.</param>
        void SetPosition(const ImVec2& pos);

        /// <summary>
        /// 노드를 선택상태로 변경합니다.
        /// </summary>
        /// <param name="append">true일 시 기존 선택상태를 유지합니다. false일 시 기존 선택상태를 초기화하고 현재 노드를 선택상태로 변경합니다.</param>
        void SetSeletion(bool append = false);

        /// <summary>
        /// 노드의 Z값을 변경합니다. Z값은 노드의 렌더링 순서를 결정합니다. 값이 높을수록 위에 렌더링됩니다.
        /// </summary>
        /// <param name="zOrder">설정할 노드의 Z값</param>
        void SetZOrder(float zOrder);

        /// <summary>
        /// 인덱스를 통해 입력 핀을 가져옵니다.
        /// </summary>
        /// <param name="index">입력 핀을 가져올 인덱스</param>
        /// <returns>해당 인덱스의 입력 핀이 있으면 해당 핀의 포인터를 반환합니다. 해당 인덱스에 입력 핀이 없으면 null포인터를 반환합니다</returns>
        const Pin* GetInputPin(size_t index = 0);

        /// <summary>
        /// 인덱스를 통해 출력 핀을 가져옵니다.
        /// </summary>
        /// <param name="index">출력 핀을 가져올 인덱스</param>
        /// <returns>해당 인덱스의 출력 핀이 있으면 해당 핀의 포인터를 반환합니다. 해당 인덱스에 출력 핀이 없으면 null포인터를 반환합니다</returns>
        const Pin* GetOutputPin(size_t index = 0);

        inline UINT64   GetNodeID()             const { return _id; }
        inline ImVec2   GetPosition()           const { return ed::GetNodePosition(_id); }
        inline ImVec2   GetSize()               const { return ed::GetNodeSize(_id); }
        inline float    GetZOrder()             const { return ed::GetNodeZPosition(_id); }
        inline bool     IsSelected()            const { return ed::IsNodeSelected(_id); }
        inline size_t   GetInputPinCount()      const { return _inputPinList.size(); }
        inline size_t   GetOutputPinCount()     const { return _outputPinList.size(); }
        inline size_t   GetTotalPinCount()      const { return GetInputPinCount() + GetOutputPinCount(); }
        inline bool     HasInputPin()           const { return !_inputPinList.empty(); }
        inline bool     HasOutputPin()          const { return !_outputPinList.empty(); }

    private:
        UINT64 _id; // 노드 ID

        std::vector<Pin> _inputPinList;  // 입력 핀 리스트
        std::vector<Pin> _outputPinList; // 출력 핀 리스트
        PinIDTable       _pinIDTable;    // ID to Pin 테이블
        PinLabelTable    _pinNameTable;  // 이름 to Pin 테이블
    };
}