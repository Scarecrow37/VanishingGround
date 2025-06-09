#pragma once
#include "NodeGraphPin.h"

class EditorNodeGraph;

namespace NodeGraph
{

    enum class NodeType
    {
        Blueprint,
        Simple,
        Tree,
        Comment,
        Houdini
    };

    class Pin;
    class Node;
    class Link;

    /*
    노드 그래프에 나타날 노드를 정의하는 클래스입니다.
    이 클래스를 상속해 Draw를 구현하여 노드의 내용을 그립니다.
    */
    class Node
    {
    public:
        Node(EditorNodeGraph* owner, const char* name);
        virtual ~Node();
    public:
        virtual void Draw() = 0;

    public:
        Pin* FindPin(ed::PinId id);

        void AddInputPin(const char* name, PinType type);
        void AddOutputPin(const char* name, PinType type);
        
        void SetPosition(const ImVec2& pos);

        inline void SetLabel(const char* name) { _name = name; }
        inline void SetHeaderColor(const ImColor& color) { _color = color; }

        inline ed::NodeId       GetNodeID()     const { return _id; }
        inline const char*      GetNodeName()   const { return _name.data(); }
        inline const ImColor&   GetNodeColor()  const { return _color; }
        inline const ImVec2&    GetNodeSize()   const { return _size; }

    protected:
        EditorNodeGraph* _owner;

        ed::NodeId       _id;
        std::string      _name;
        ImColor          _color;
        ImVec2           _size;
        std::vector<Pin> _inputPinList;
        std::vector<Pin> _outputPinList;
        std::unordered_map<UINT64, NodeGraph::Pin*> _pinTable; // Pin ID to Pin mapping

        std::string _serialData;
        std::string SavedState;
    };
}