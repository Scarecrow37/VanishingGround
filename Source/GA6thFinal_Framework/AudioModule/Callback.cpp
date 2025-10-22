#include "pch.h"
#include "Callback.h"

namespace Audio
{
    Callback::Callback(const std::function<void(AudioHandle handle)>& onBufferEnd) : _onBufferEnd(onBufferEnd) {}

    void Callback::OnBufferEnd(void* pBufferContext)
    {
        if (_onBufferEnd)
            _onBufferEnd(_handle);
    }

    void Callback::SetHandle(const AudioHandle& handle)
    {
        _handle = handle;
    }
} // namespace Audio