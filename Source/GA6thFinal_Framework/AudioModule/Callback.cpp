#include "pch.h"
#include "Callback.h"

namespace Audio
{
    Callback::Callback() = default;

    void Callback::OnBufferEnd(void* pBufferContext)
    {
        if (_onBufferEnd)
            _onBufferEnd();
    }
} // namespace Audio