#pragma once

namespace Audio
{
    class EManager;

    class IncreaseGenerationCallback final : public IXAudio2VoiceCallback
    {
    public:
        IncreaseGenerationCallback();
        IncreaseGenerationCallback(EManager* manager, const Handle& handle);

        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
        void STDMETHODCALLTYPE OnStreamEnd() override {}
        void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override {}
        void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
        void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override {}
        void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT) override {}

    private:
        EManager* _manager;
        Handle _handle;
    };
} // namespace Audio