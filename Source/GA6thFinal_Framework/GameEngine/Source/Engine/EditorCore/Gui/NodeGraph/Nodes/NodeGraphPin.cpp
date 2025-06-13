#include "pch.h"
#include "NodeGraphPin.h"

namespace NodeGraph
{
    Pin::Pin(const char* name, ed::PinKind kind, LinkFilter filter)
        : _id(NodeGraph::GetUniqueID())
        , _ownerNodeID(NodeGraph::GetCurrentNode()->GetNodeID())
        , _label(name)
        , _kind(kind) 
        , _linkFilter(filter)
        , _maxLinkCount(-1) 
    {
    }
    Pin::~Pin() 
    {
    }

    Link* Pin::AddLink(Pin* dest, const ImColor& linkColor)
    {
        Link* link = nullptr;
        bool thisFull = IsLinkFull();
        bool destFull   = dest->IsLinkFull();
        if (false == thisFull && false == destFull)
        {
            auto context = NodeGraph::GetCurrentNodeGraphContext();
            link = context->CreateLink(this->GetPinID(), dest->GetPinID(), linkColor);
            dest->_linkIDVector.push_back(link->GetLinkID());
            this->_linkIDVector.push_back(link->GetLinkID());
        }
        return link;
    }

    bool Pin::RemoveLink(UINT64 linkID)
    {
        auto it = std::find(_linkIDVector.begin(), _linkIDVector.end(), linkID);
        if (it != _linkIDVector.end())
        {
            _linkIDVector.erase(it);
            NodeGraph::GetCurrentNodeGraphContext()->RemoveLinkFromLinkID(linkID);
            return true;
        }
        return false;
    }
    Link* Pin::GetLinkToIndex(int index) const
    {
        Link* link = nullptr;
        if (index < _linkIDVector.size())
        {
            UINT64 id = _linkIDVector[index];
            auto context = NodeGraph::GetCurrentNodeGraphContext();
            context->FindLinkFromLinkID(id, &link);
        }
        return link;
    }
    bool Pin::CanLink(Pin* dest) const
    {
        if (nullptr != dest && nullptr != _linkFilter)
        {
            return _linkFilter(this, dest);
        }
        return false;
    }
    /* Static */
    bool Pin::IsSameOwner(const Pin* a, const Pin* b)
    {
        return a->GetOwnerNodeID() == b->GetOwnerNodeID();
    }
    bool Pin::IsSameKind(const Pin* a, const Pin* b)
    {
        return a->GetPinKind() == b->GetPinKind();
    }
    bool Pin::IsSamePin(const Pin* a, const Pin* b)
    {
        return a == b;
    }
} // namespace NodeGraph