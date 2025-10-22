#pragma once

namespace Audio
{
    class System;

    class Callback final : public IXAudio2VoiceCallback
    {
    public:
        explicit Callback(const std::function<void(AudioHandle handle)>& onBufferEnd);

        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
        void STDMETHODCALLTYPE OnStreamEnd() override {}
        void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override {}
        void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
        void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override {}
        void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT) override {}

        void SetHandle(const AudioHandle& handle);

    private:
        std::function<void(AudioHandle handle)> _onBufferEnd;
        AudioHandle                             _handle;
    };
} // namespace Audio