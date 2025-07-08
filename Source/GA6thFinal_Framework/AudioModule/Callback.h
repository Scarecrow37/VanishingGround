#pragma once

namespace Audio
{
    class System;

    class Callback final : public IXAudio2VoiceCallback
    {
    public:
        Callback();

        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
        void STDMETHODCALLTYPE OnStreamEnd() override {}
        void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override {}
        void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
        void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override {}
        void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT) override {}

        void SetOnBufferEndCallback(std::function<void()> callback) { _onBufferEnd = std::move(callback); }

    private:
        std::function<void()> _onBufferEnd;
    };
} // namespace Audio