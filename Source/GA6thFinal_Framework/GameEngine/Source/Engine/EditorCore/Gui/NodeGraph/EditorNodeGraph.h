#pragma once
#include "Utilities/builders.h"
#include "Utilities/drawing.h"
#include "Utilities/widgets.h"
#include "Nodes/NodeGraphNode.h"
#include "Nodes/NodeGraphLink.h"

namespace NodeGraph
{
    constexpr ImColor CANT_LINK_LABEL_COLOR(100, 20, 20, 200); // 링크를 할 수 없는 툴팁 바의 색
    constexpr ImColor CAN_LINK_LABEL_COLOR(20, 100, 20, 200);  // 링크를 할 수 있는 툴팁 바의 색
}

class NodeGraphContext
    : public ReflectSerializer
{
public:
    struct State
    {
        bool isProcessingNewNode = false;

        NodeGraph::Pin* NewLinkPin = nullptr;       // 링크를 하기 나온 핀
        NodeGraph::Pin* NewNodeLinkPin = nullptr;   // 새로운 노드에 연결할 핀
    };
public:
    NodeGraphContext();
    ~NodeGraphContext();

public:
    void Render();
    void Clear();

public:
    template <typename T>
    T* AddNode();

    /// <summary>
    /// 노드ID를 통해 노드를 찾습니다.
    /// </summary>
    /// <param name="nodeID">[IN] 찾을 노드의 ID값</param>
    /// <param name="ppNode">[OUT] 찾은 노드의 포인터를 반환합니다.</param>
    /// <returns>찾는데 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool FindNodeFromNodeID(IN UINT64 nodeID, OUT NodeGraph::Node** ppNode);
    /// <summary>
    /// 핀ID를 통해 노드를 찾습니다.
    /// </summary>
    /// <param name="pinID">[IN] 찾을 핀의 ID값</param>
    /// <param name="ppNode">[OUT] 찾은 노드의 포인터를 반환합니다.</param> 
    /// <returns>찾는데 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool FindNodeFromPinID(IN UINT64 pinID, OUT NodeGraph::Node** ppNode);
    /// <summary>
    /// 핀ID를 통해 핀을 찾습니다.
    /// </summary>
    /// <param name="pinID">[IN]찾을 핀의 ID값</param> 
    /// <param name="ppPin">[OUT]찾은 핀의 포인터를 반환합니다.</param> 
    /// <returns>찾는데 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool FindPinFromPinID(IN UINT64 pinID, OUT NodeGraph::Pin** ppPin);
    /// <summary>
    /// 링크ID를 통해 링크를 찾습니다.
    /// </summary>
    /// <param name="linkID">[IN]찾을 링크의 ID값</param>
    /// <param name="ppLink">[OUT]찾은 링크의 포인터를 반환합니다.</param> 
    /// <returns>찾는데 성공하면 true, 실패하면 false를 반환합니다.</returns> 
    bool FindLinkFromLinkID(IN UINT64 linkID, OUT NodeGraph::Link** ppLink);
    /// <summary>
    /// 링크ID를 통해 해당 링크의 시작 핀과 끝 핀을 찾습니다.
    /// </summary>
    /// <param name="linkID">[IN]찾을 링크의 ID값</param>
    /// <param name="ppStartPin">[OUT]해당 링크의 시작 핀을 반환합니다.</param>
    /// <param name="ppEndPin">[OUT]해당 링크의 끝 핀을 반환합니다.</param>
    /// <returns>찾는데 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool FindPinsFromLinkID(IN UINT64 linkID, OUT NodeGraph::Pin** ppStartPin, OUT NodeGraph::Pin** ppEndPin);
    /// <summary>
    /// 노드ID를 통해 노드를 제거합니다.
    /// </summary>
    /// <param name="nodeID">[IN] 제거할 노드의 ID값</param>
    /// <returns>삭제에 성공하면 true, 실패하면 false를 반환합니다.</returns> 
    bool RemoveNodeFromNodeID(IN UINT64 nodeID);
    /// <summary>
    /// 링크ID를 통해 링크를 제거합니다.
    /// </summary>
    /// <param name="linkID">[IN] 제거할 링크의 ID값</param>
    /// <returns>삭제에 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool RemoveLinkFromLinkID(IN UINT64 linkID);

    inline void SetCurrentContext()     { ed::SetCurrentEditor(_editor); }

    inline const State& GetState()      { return _state; }
    inline UINT64       GetUniqueID()   { return _uniqueID++; }

private:
    void ProcessNodes();
    void ProcessLinks();

    void ProcessCreate();
    void ProcessCreateLink();
    void ProcessCreateNode();

    void ProcessRemove();
    void ProcessRemoveNode();
    void ProcessRemoveLink();

    void ProcessPopupContext();

    void ShowLabel(const char* label, const ImColor& bgColor, const ImVec4& textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

private:
    ed::EditorContext* _editor;
    State              _state;
    UINT64             _uniqueID;

    std::vector<NodeGraph::Node*> _nodeVector;
    std::vector<NodeGraph::Link*> _linkVector;
    std::unordered_map<UINT64, NodeGraph::Node*> _nodeTable;
    std::unordered_map<UINT64, NodeGraph::Link*> _linkTable;

public:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string SerializeData           = "";
    float       LinkThickness           = 2.0f;
    enum LinkDataIndex { R, G, B, A, THICKNESS, };
    std::array<float, 5> DefaultNewLinkData = {0.0f, 1.0f, 0.0f, 0.5f, 2.0f};
    std::array<float, 5> RejectNewLinkData  = {1.0f, 0.2f, 0.2f, 1.0f, 1.0f};
    std::array<float, 5> AcceptNewLinkData  = {0.2f, 1.0f, 0.2f, 1.0f, 4.0f};
    REFLECT_FIELDS_END(NodeGraphContext)

};

template <typename T>
inline T* NodeGraphContext::AddNode()
{
    static_assert(std::is_base_of<NodeGraph::Node, T>::value, "T must be derived from NodeGraph::Node");
    NodeGraph::Node* node = nullptr;
    ed::SetCurrentEditor(_editor);
    {
        node = new T();
        node->OnCreate();
        _nodeVector.push_back(node);
        _nodeTable[node->GetNodeID()] = node;
    }
    return static_cast<T*>(node);
}
