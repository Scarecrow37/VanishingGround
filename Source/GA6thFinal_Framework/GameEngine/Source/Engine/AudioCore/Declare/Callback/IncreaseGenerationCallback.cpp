#include "pch.h"
#include "IncreaseGenerationCallback.h"

namespace Audio
{
    IncreaseGenerationCallback::IncreaseGenerationCallback() = default;

    IncreaseGenerationCallback::IncreaseGenerationCallback(EManager* manager, const Handle& handle)
        : _manager(manager), _handle(handle)
    {
    }

    void STDMETHODCALLTYPE IncreaseGenerationCallback::OnBufferEnd(void* pBufferContext)
    {
        _manager->ReleaseVoice(_handle);
    }
} // namespace Audio