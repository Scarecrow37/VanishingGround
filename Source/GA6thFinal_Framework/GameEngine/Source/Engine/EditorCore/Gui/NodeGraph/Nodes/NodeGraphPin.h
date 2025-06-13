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

    public:
        // 같은 노드를 참조하는 핀인지 확인합니다.
        static bool IsSameOwner(const Pin* a, const Pin* b);
        // 같은 종류의 핀인지 확인합니다. (입력, 출력)
        static bool IsSameKind(const Pin* a, const Pin* b);
        // 같은 종류의 핀인지 확인합니다. (타입)
        static bool IsSameType(const Pin* a, const Pin* b);
        // 같은 핀인지 확인합니다.
        static bool IsSamePin(const Pin* a, const Pin* b);
        
    };
} // namespace NodeGraph