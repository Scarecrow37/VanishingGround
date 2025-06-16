#include "pch.h"
#include "BTRootNode.h"

namespace BehaviorTree
{
    RootNode::RootNode() 
    {
        SetLabel("Root Node");
        auto filter = [](const NodeGraph::Pin* self, const NodeGraph::Pin* dest) -> bool {
            // 예시: 같은 종류의 핀은 연결할 수 없습니다.
            if (self->GetPinKind() == dest->GetPinKind())
            {
                return false;
            }
            return true;
        };

        auto pin = AddPin("OutputPin", ed::PinKind::Output, filter);
        pin->SetMaxLinkCount(1); // 최대 하나
    }

    RootNode::~RootNode() 
    {
    }

} // namespace BehaviorTree