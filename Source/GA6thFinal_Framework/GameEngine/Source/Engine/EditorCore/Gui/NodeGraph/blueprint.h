#pragma once

class Texture;

class blueprint
{
public:
    blueprint() = default;
    ~blueprint();

public:
    void OnStart();
    void OnFrame(float deltaTime);
    void OnStop();
    void TestUpdate();

private:
    int GetNextId() { return _nextId++; }
    ed::LinkId GetNextLinkId();
    float GetTouchProgress(ed::NodeId id);
    ImColor GetIconColor(NodeGraph::PinType type);

    NodeGraph::Node* FindNode(ed::NodeId id);
    NodeGraph::Link* FindLink(ed::LinkId id);
    NodeGraph::Pin* FindPin(ed::PinId id);

    bool IsPinLinked(ed::PinId id);
    bool CanCreateLink(NodeGraph::Pin* a, NodeGraph::Pin* b);

    void TouchNode(ed::NodeId id);
    void UpdateTouch();
    void BuildNode(NodeGraph::Node* node);
    void BuildNodes();

    NodeGraph::Node* SpawnInputActionNode();
    NodeGraph::Node* SpawnBranchNode();
    NodeGraph::Node* SpawnDoNNode();
    NodeGraph::Node* SpawnOutputActionNode();
    NodeGraph::Node* SpawnPrintStringNode();
    NodeGraph::Node* SpawnMessageNode();
    NodeGraph::Node* SpawnSetTimerNode();
    NodeGraph::Node* SpawnLessNode();
    NodeGraph::Node* SpawnWeirdNode();
    NodeGraph::Node* SpawnTraceByChannelNode();
    NodeGraph::Node* SpawnTreeSequenceNode();
    NodeGraph::Node* SpawnTreeTaskNode();
    NodeGraph::Node* SpawnTreeTask2Node();
    NodeGraph::Node* SpawnComment();
    NodeGraph::Node* SpawnHoudiniTransformNode();
    NodeGraph::Node* SpawnHoudiniGroupNode();    

    void DrawPinIcon(const NodeGraph::Pin& pin, bool connected, int alpha);
    void ShowStyleEditor(bool* show = nullptr);
    void ShowLeftPane(float paneWidth);
    
private:
    ImRect ImGui_GetItemRect();
    ImRect ImRect_Expanded(const ImRect& rect, float x, float y);
    bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

private:
    ed::EditorContext*      _editor = nullptr;
    int                     _nextId = 1;
    const int               _pinIconSize = 24;
    std::vector<NodeGraph::Node>       _nodes;
    std::vector<NodeGraph::Link>       _links;
    ImTextureID             _headerBackground = 0;
    ImTextureID             _saveIcon = 0;
    ImTextureID             _restoreIcon = 0;
    const float             _touchTime = 1.0f;
    std::map<ed::NodeId, float, NodeGraph::NodeIdLess> _nodeTouchTime;
    bool                    _showOrdinals = false;
};