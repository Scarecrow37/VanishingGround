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
    public:
        Node();
        virtual ~Node();

    public:
        virtual void Draw() = 0;
        virtual void OnCreate() {};
        virtual void OnNodePopup() {};
        virtual void OnPinPopup(Pin* pin) {};

    public:
        Pin* FindPin(ed::PinId id);
        Pin* AddPin(const char* name, const char* type, ed::PinKind kind);
        
        void SetPosition(const ImVec2& pos);

        inline void SetLabel(const char* name) { _label = name; }

        inline ed::NodeId       GetNodeID()     const { return _id; }
        inline const char*      GetNodeLabel()  const { return _label.data(); }
        inline const ImVec2&    GetNodeSize()   const { return _size; }

        inline ImVec2 GetPosition() const { return ed::GetNodePosition(_id); }
        inline ImVec2 GetSize() const { return ed::GetNodeSize(_id); }

    protected:
        ed::NodeId       _id;
        std::string      _label;
        ImVec2           _size;
        
        std::vector<Pin> _inputPinList;
        std::vector<Pin> _outputPinList;
        std::vector<Pin*> _totalPinList;
        std::unordered_map<UINT64, NodeGraph::Pin*> _pinTable; // Pin ID to Pin mapping

        std::string _serialData;
        std::string SavedState;
    };
}