#pragma once

class EditorNodeGraph
{
public:
    EditorNodeGraph();
    ~EditorNodeGraph();

public:
    void Render();

public:
    NodeGraph::Node* FindNode(ed::NodeId id);
    NodeGraph::Link* FindLink(ed::LinkId id);

    /* SerializeFunc */
    void        SaveData(const char* data, size_t size = 0);
    void        LoadData(const std::string& data);
    const char* SaveNodeSettingsToMemory();
    void        LoadNodeSettingsFromMemory(const std::string& data);
    const char* GetNodeSettingsData();

private:
    ed::EditorContext* _editor;

    std::vector<NodeGraph::Node> _nodeVector;
    std::vector<NodeGraph::Link> _linkedVector;

    std::string _serializeData;

};
