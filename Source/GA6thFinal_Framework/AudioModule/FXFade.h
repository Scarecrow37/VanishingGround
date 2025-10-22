#pragma once

class __declspec(uuid("3c1474dc-ee09-465d-8904-2f38bc3715a1")) FXFade;
EXTERN_C const GUID DECLSPEC_SELECTANY                         CLSID_FXFade = __uuidof(FXFade);

constexpr XAPO_REGISTRATION_PROPERTIES FX_FADE_REGISTRATION_PROPERTIES = {
    .clsid                = __uuidof(FXFade),
    .FriendlyName         = L"Fade XAPO",
    .CopyrightInfo        = L"Made by Dosuha",
    .MajorVersion         = 1,
    .MinorVersion         = 0,
    .Flags                = XAPOBASE_DEFAULT_FLAG,
    .MinInputBufferCount  = XAPOBASE_DEFAULT_BUFFER_COUNT,
    .MaxInputBufferCount  = XAPOBASE_DEFAULT_BUFFER_COUNT,
    .MinOutputBufferCount = XAPOBASE_DEFAULT_BUFFER_COUNT,
    .MaxOutputBufferCount = XAPOBASE_DEFAULT_BUFFER_COUNT};

namespace Audio
{
    class FXFade : public CXAPOParametersBase
    {
    public:
        explicit FXFade(FadeInitParameter initParameter);

        HRESULT LockForProcess(UINT32 InputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters) noexcept override;

        void Process(UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled) noexcept override;

    private:
        FadeParameter _parameterBlocks[3];

        UINT32 _channels;
        UINT32 _bytePerSample;
        DWORD  _sampleRate;

        float _beginGain;
        float _endGain;
        float _duration;

        float _minGain;
        float _maxGain;

        float _currentGain;
        float _stepOfGain;
    };
}