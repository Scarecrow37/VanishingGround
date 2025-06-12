#pragma once

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;

    class Pin
    {
    public:
        Pin(const char* label, const char* type, ed::PinKind kind);
        ~Pin();

    public:
        inline UINT64      GetPinID()       const { return _id; }
        inline UINT64      GetOwnerNodeID() const { return _ownerNodeID; }
        inline const char* GetPinLabel()    const { return _label.data(); }
        inline const char* GetPinType()     const { return _type.data(); }
        inline ed::PinKind GetPinKind()     const { return _kind; }

    private:
        UINT64      _id;
        UINT64      _ownerNodeID;
        std::string _label;
        std::string _type;
        ed::PinKind _kind;
        
    };
} // namespace NodeGraph