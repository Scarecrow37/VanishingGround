#pragma once
#include "Utilities/builders.h"
#include "Utilities/drawing.h"
#include "Utilities/widgets.h"
#include "Nodes/NodeGraphNode.h"
#include "Nodes/NodeGraphLink.h"

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

    NodeGraph::Node* FindNodeFromNodeID(UINT64 nodeID);
    NodeGraph::Node* FindNodeFromPinID(UINT64 pinID);
    NodeGraph::Pin*  FindPinFromPinID(UINT64 pinID);

    /* SerializeFunc */
    void        SaveData(const char* data, size_t size = 0);
    void        LoadData(const std::string& data);
    const char* SaveNodeSettingsToMemory();
    void        LoadNodeSettingsFromMemory(const std::string& data);
    const char* GetNodeSettingsData();

    inline void SetCurrentContext()     { ed::SetCurrentEditor(_editor); }

    inline const State& GetState()      { return _state; }
    inline UINT64       GetUniqueID()   { return _uniqueID++; }

private:

    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;

    void ProcessNodes();
    void ProcessLinks();

    void ProcessCreate();
    void ProcessCreateLink();
    void ProcessCreateNode();

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
