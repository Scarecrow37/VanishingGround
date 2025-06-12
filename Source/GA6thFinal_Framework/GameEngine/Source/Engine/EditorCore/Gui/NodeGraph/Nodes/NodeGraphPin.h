#pragma once

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;

    class Pin
    {
    public:
        Pin(const char* name, const char* type, ed::PinKind kind);
        ~Pin();

    public:
        inline ed::PinId   GetPinID()       const { return _id; }
        inline ed::NodeId  GetOwnerNodeID() const { return _ownerNodeID; }
        inline const char* GetPinName()     const { return _name.data(); }
        inline const char* GetPinType()     const { return _type.data(); }
        inline ed::PinKind GetPinKind()     const { return _kind; }

    private:
        ed::PinId   _id;
        ed::NodeId  _ownerNodeID;
        std::string _name;
        std::string _type;
        ed::PinKind _kind;
        
    };
} // namespace NodeGraph