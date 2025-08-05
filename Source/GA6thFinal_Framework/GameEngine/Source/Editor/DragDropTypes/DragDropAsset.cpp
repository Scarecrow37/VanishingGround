#include "pch.h"
#include "DragDropAsset.h"

const File::Path& DragDropAsset::Data::GetPath() const
{
    if (pPath)
    {
        return *pPath;
    }
    else if (pContext && !pContext->expired())
    {
        auto context = pContext->lock();
        if (context)
        {
            return context->GetPath();
        }
    }
    return File::NULL_PATH;
}

const File::Guid& DragDropAsset::Data::GetGuid() const
{
    if (pGuid)
    {
        return *pGuid;
    }
    else if (pContext && !pContext->expired())
    {
        auto context = pContext->lock();
        if (context)
        {
            return context->GetMeta().GetGuid();
        }
    }
    return File::NULL_GUID;
}
