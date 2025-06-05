#pragma once

namespace NodeGraph
{
    enum class PinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };

    class Pin;
    class Node;
    class Link;

    class Pin
    {
    public:
        Pin(int id, const char* name, PinType type, ed::PinKind kind, Node* owner)
            : _id(id), _name(name), _type(type), _kind(kind), _owner(owner)
        {}

    public:
        inline Node*       GetOwnerNode()   const { return _owner; }
        inline ed::PinId   GetPinID()       const { return _id; }
        inline const char* GetPinName()     const { return _name.data(); }
        inline ed::PinKind GetPinKind()     const { return _kind; }
        inline PinType     GetPinType()     const { return _type; }

    private:
        Node*       _owner;
        ed::PinId   _id;
        std::string _name;
        ed::PinKind _kind;
        PinType     _type;
    };
} // namespace NodeGraph