#pragma once

namespace NodeGraph
{
    class Pin;
    class Node;
    class Link;
    using LinkFilter = std::function<bool(const Pin*, const Pin*)>;

    /// <summary>
    /// <para>NodeGraph애서 Pin을 정의하는 클래스입니다. </para>
    /// <para>Pin은 노드의 입력과 출력을 정의하며, 노드 간의 연결을 가능하게 합니다. </para>
    /// <para>사용자가 링크 허용 타입을 직접 추가해줘야 하며, 최대 링크 갯수를 제한할 수 있습니다. </para>
    /// </summary>
    class Pin
    {
        
    public:
        /// <param name="label">핀의 이름입니다. 사용자가 핀을 식별하거나 렌더링하기 위해 사용할 수 있습니다.</param>
        /// <param name="kind">핀의 종류입니다. 입력 핀인지 출력 핀인지 구분합니다.</param>
        /// <param name="filter">핀의 링크 필터 함수입니다. 이 함수는 핀 간의 연결을 허용할지 여부를 결정합니다.</param>
        Pin(const char* label, ed::PinKind kind, LinkFilter filter);
        ~Pin();

    public:
        /// <summary>
        /// 현재 핀에서 대상 핀으로의 링크를 추가합니다. 최대 링크 수를 넘기면 만들지 못합니다.
        /// </summary>
        /// <param name="dest">대상 핀의 포인터</param>
        /// <param name="linkColor">링크의 색상</param>
        /// <returns>링크를 만드는데 성공했다면 해당 링크의 포인터를 반환합니다. 만들지 못했다면 null포인터를 반환합니다.</returns>
        Link* AddLink(Pin* dest, const ImColor& linkColor);

        /// <summary>
        /// 현재 핀에서 링크를 제거합니다.
        /// </summary>
        /// <param name="linkID">제거할 링크의 ID값 입니다.</param>
        /// <returns>제거에 성공하면 true, 해당 링크가 없다면 false를 반환합니다.</returns>
        bool RemoveLink(UINT64 linkID);

        Link* GetLinkToIndex(int index) const;

        /// <summary>
        /// 대상 핀과 링크가 가능한지 판별합니다.
        /// </summary>
        /// <param name="dest">대상 핀</param>
        /// <returns></returns>
        bool CanLink(Pin* dest) const;

        /// <summary>
        /// 최대 링크 개수를 설정합니다. -1은 무제한을 의미합니다.
        /// </summary>
        /// <param name="maxLinkCount">최대 링크 수</param>
        inline void SetMaxLinkCount(size_t maxLinkCount) { _maxLinkCount = maxLinkCount; }
       
        /// <summary>
        /// 핀의 태그를 추가합니다.
        /// </summary>
        /// <param name="tag">태그 이름</param>
        inline void AddTag(const char* tag) { _tagSet.insert(tag); }

        /// <summary>
        /// 핀에 해당 태그가 존재하는지 확인합니다.
        /// </summary>
        /// <param name="tag">태그 이름</param>
        /// <returns>존재하면 true, 존재하지 않으면 false를 반환합니다.</returns>
        inline bool IsExistTag(const char* tag) const { return _tagSet.find(tag) != _tagSet.end(); }

        inline UINT64      GetPinID()           const { return _id; }
        inline UINT64      GetOwnerNodeID()     const { return _ownerNodeID; }
        inline const char* GetPinLabel()        const { return _label.data(); }
        inline ed::PinKind GetPinKind()         const { return _kind; }
        inline int         GetMaxLinkCount()    const { return _maxLinkCount; }
        inline size_t      GetLinkCount()       const { return _linkIDVector.size(); }
        inline bool        IsLinkFull()         const { return (_maxLinkCount != -1 && _linkIDVector.size() >= _maxLinkCount); }
        
    private:
        UINT64      _id;
        UINT64      _ownerNodeID;
        std::string _label;
        ed::PinKind _kind;
        size_t      _maxLinkCount; // 핀에 연결할 수 있는 최대 링크 개수. (-1은 무제한을 의미)
        LinkFilter  _linkFilter;   // 링크 필터 함수. 핀 간의 연결을 허용할지 여부를 결정합니다. (첫번 째 인자의 핀은 나, 두번째 인자는 연결하려는 핀입니다.)

        std::unordered_set<std::string> _tagSet; // 핀의 태그를 저장하는 집합. 태그는 핀 간의 연결을 허용할 때 사용됩니다.
        std::vector<UINT64> _linkIDVector;
       

    public:
        // 같은 노드를 참조하는 핀인지 확인합니다.
        static bool IsSameOwner(const Pin* a, const Pin* b);
        // 같은 종류의 핀인지 확인합니다. (입력, 출력)
        static bool IsSameKind(const Pin* a, const Pin* b);
        // 같은 핀인지 확인합니다.
        static bool IsSamePin(const Pin* a, const Pin* b);
        
    };
} // namespace NodeGraph