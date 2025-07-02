#pragma once
#include "NodeEditor.h"

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
    ImColor GetIconColor(PinType type);

    Node* FindNode(ed::NodeId id);
    Link* FindLink(ed::LinkId id);
    Pin* FindPin(ed::PinId id);

    bool IsPinLinked(ed::PinId id);
    bool CanCreateLink(Pin* a, Pin* b);

    void TouchNode(ed::NodeId id);
    void UpdateTouch();
    void BuildNode(Node* node);
    void BuildNodes();

    Node* SpawnInputActionNode();
    Node* SpawnBranchNode();
    Node* SpawnDoNNode();
    Node* SpawnOutputActionNode();
    Node* SpawnPrintStringNode();
    Node* SpawnMessageNode();
    Node* SpawnSetTimerNode();
    Node* SpawnLessNode();
    Node* SpawnWeirdNode();
    Node* SpawnTraceByChannelNode();
    Node* SpawnTreeSequenceNode();
    Node* SpawnTreeTaskNode();
    Node* SpawnTreeTask2Node();
    Node* SpawnComment();
    Node* SpawnHoudiniTransformNode();
    Node* SpawnHoudiniGroupNode();    

    void DrawPinIcon(const Pin& pin, bool connected, int alpha);
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
    std::vector<Node>       _nodes;
    std::vector<Link>       _links;
    ImTextureID             _headerBackground = 0;
    ImTextureID             _saveIcon = 0;
    ImTextureID             _restoreIcon = 0;
    const float             _touchTime = 1.0f;
    std::map<ed::NodeId, float, NodeIdLess> _nodeTouchTime;
    bool                    _showOrdinals = false;
};