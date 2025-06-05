#pragma once
#include "Utilities/builders.h"
#include "Utilities/drawing.h"
#include "Utilities/widgets.h"
#include "Nodes/NodeGraphNode.h"
#include "Nodes/NodeGraphLink.h"

class EditorNodeGraph 
    : public ReflectSerializer
{
    using ID = uintptr_t;

public:
    struct State
    {
        NodeGraph::Pin* NewLinkPin = nullptr;
    };
public:
    EditorNodeGraph();
    ~EditorNodeGraph();

public:
    void Render();

public:
    template <typename T>
    T* AddNode(const char* label);

    NodeGraph::Node* FindNode(ed::NodeId id);
    NodeGraph::Link* FindLink(ed::LinkId id);

    /* SerializeFunc */
    void        SaveData(const char* data, size_t size = 0);
    void        LoadData(const std::string& data);
    const char* SaveNodeSettingsToMemory();
    void        LoadNodeSettingsFromMemory(const std::string& data);
    const char* GetNodeSettingsData();

    inline void SetCurrentContext()     { ed::SetCurrentEditor(_editor); }

    inline const State& GetState()      { return _state; }
    inline int          GetUniqueID()   { return _uniqueID++; }

private:
    virtual void SerializedReflectEvent() override;
    virtual void DeserializedReflectEvent() override;

    void DrawNodes();

private:
    ed::EditorContext* _editor;
    State              _state;
    int                _uniqueID;

    std::vector<NodeGraph::Node*> _nodeVector;
    std::vector<NodeGraph::Link*> _linkVector;
    std::unordered_map<ID, NodeGraph::Node*> _nodeTable;
    std::unordered_map<ID, NodeGraph::Link*> _linkTable;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string SerializeData;
    REFLECT_FIELDS_END(EditorNodeGraph)
};

template <typename T>
inline T* EditorNodeGraph::AddNode(const char* label)
{
    static_assert(std::is_base_of<NodeGraph::Node, T>::value, "T must be derived from NodeGraph::Node");
    T* node = nullptr;
    ed::SetCurrentEditor(_editor);
    {
        node = new T(this, label);
        _nodeVector.push_back(node);
        _nodeTable[node->GetNodeID().Get()] = node;
    }
    return node;
}
