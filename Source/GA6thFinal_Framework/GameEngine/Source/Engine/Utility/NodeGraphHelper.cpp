#include "pch.h"
#include "NodeGraphHelper.h"

namespace NodeGraph
{
    bool CanLink(Pin* from, Pin* to)
    {
        if (nullptr == from || nullptr == to)
        {
            return false;
        }

        bool isSamePin   = (from == to);
        bool isSameKind  = (from->GetPinKind() == to->GetPinKind());
        bool isSameType  = (from->GetPinType() == to->GetPinType());
        bool isSameOwner = (from->GetOwnerNode() == to->GetOwnerNode());

        if (true == isSamePin || true == isSameKind || true == isSameType || true == isSameOwner)
            return false;

        return true;
    }

    ImU32 GetColor4ToImU32(const ImVec4& color)
    {
        int R = static_cast<int>(255.0f * ImClamp(color.x, 0.0f, 1.0f));
        int G = static_cast<int>(255.0f * ImClamp(color.y, 0.0f, 1.0f));
        int B = static_cast<int>(255.0f * ImClamp(color.z, 0.0f, 1.0f));
        int A = static_cast<int>(255.0f * ImClamp(color.w, 0.0f, 1.0f));
        return IM_COL32(R, G, B, A);
    }

    ImU32 GetColor4ToImU32(int* color)
    {
        if (nullptr == color)
        {
            return ImU32();
        }
        int R = ImClamp(color[0], 0, 255);
        int G = ImClamp(color[1], 0, 255);
        int B = ImClamp(color[2], 0, 255);
        int A = ImClamp(color[3], 0, 255);
        return IM_COL32(R, G, B, A);
    }

    ImU32 GetColor4ToImU32(float r, float g, float b, float a)
    {
        int R = static_cast<int>(255.0f * ImClamp(r, 0.0f, 1.0f));
        int G = static_cast<int>(255.0f * ImClamp(g, 0.0f, 1.0f));
        int B = static_cast<int>(255.0f * ImClamp(b, 0.0f, 1.0f));
        int A = static_cast<int>(255.0f * ImClamp(a, 0.0f, 1.0f));
        return IM_COL32(R, G, B, A);
    }

} // namespace NodeGraph

