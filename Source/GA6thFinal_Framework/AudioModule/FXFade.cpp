#include "pch.h"
#include "FXFade.h"

#include <cassert>

namespace Audio
{
    FXFade::FXFade(const FadeInitParameter initParameter)
        : CXAPOParametersBase(&FX_FADE_REGISTRATION_PROPERTIES, reinterpret_cast<BYTE*>(_parameterBlocks),
                              sizeof(FadeParameter), FALSE),
          _channels(0), _bytePerSample(0), _sampleRate(0), _beginGain(initParameter.BeginVolume),
          _endGain(initParameter.EndVolume), _duration(initParameter.Duration),
          _minGain((std::min)(initParameter.BeginVolume, initParameter.EndVolume)),
          _maxGain((std::max)(initParameter.BeginVolume, initParameter.EndVolume)),
          _currentGain(initParameter.BeginVolume), _stepOfGain(0)
    {
        assert(_minGain >= FX_FADE_MIN_VOLUME);
        assert(_maxGain <= FX_FADE_MAX_VOLUME);
        assert(_duration >= FX_FADE_MIN_DURATION);
    }

    HRESULT FXFade::LockForProcess(UINT32 InputLockedParameterCount,
                                   const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount,
                                   const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters) noexcept
    {
        assert(!IsLocked());
        assert(InputLockedParameterCount == 1);
        assert(OutputLockedParameterCount == 1);
        assert(pInputLockedParameters != NULL);
        assert(pOutputLockedParameters != NULL);
        assert(pInputLockedParameters[0].pFormat != NULL);
        assert(pOutputLockedParameters[0].pFormat != NULL);

        _channels   = pInputLockedParameters[0].pFormat->nChannels;
        _bytePerSample = pInputLockedParameters[0].pFormat->wBitsPerSample >> 3;
        _sampleRate    = pInputLockedParameters[0].pFormat->nSamplesPerSec;
        const float fadeFrame = _duration * static_cast<float>(_sampleRate);
        _stepOfGain           = (_endGain - _beginGain) / fadeFrame;

        return CXAPOBase::LockForProcess(InputLockedParameterCount, pInputLockedParameters, OutputLockedParameterCount,
                                         pOutputLockedParameters);
    }

    void FXFade::Process(UINT32 InputProcessParameterCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled) noexcept
    {
        BYTE* parameters = BeginProcess();

        assert(IsLocked());
        assert(InputProcessParameterCount == 1);
        assert(OutputProcessParameterCount == 1);
        assert(NULL != pInputProcessParameters);
        assert(NULL != pOutputProcessParameters);

        const FadeParameter* param = reinterpret_cast<FadeParameter*>(parameters);

        assert(param->Direction == FadeDirection::FORWARD || param->Direction == FadeDirection::BACKWARD);
        assert(_sampleRate > 0);

        XAPO_BUFFER_FLAGS inFlags  = pInputProcessParameters[0].BufferFlags;
        XAPO_BUFFER_FLAGS outFlags = pOutputProcessParameters[0].BufferFlags;

        // assert buffer flags are legitimate
        assert(inFlags == XAPO_BUFFER_VALID || inFlags == XAPO_BUFFER_SILENT);
        assert(outFlags == XAPO_BUFFER_VALID || outFlags == XAPO_BUFFER_SILENT);

        // check input APO_BUFFER_FLAGS
        switch (inFlags)
        {
        case XAPO_BUFFER_VALID: {
            BYTE* pvSrc = static_cast<BYTE*>(pInputProcessParameters[0].pBuffer);
            assert(pvSrc != NULL);

            BYTE* pvDst = static_cast<BYTE*>(pOutputProcessParameters[0].pBuffer);
            assert(pvDst != NULL);

            UINT32 validFrameCount = pInputProcessParameters[0].ValidFrameCount;

            float workStation = 0.0;

            for (UINT32 frame = 0; frame < validFrameCount; ++frame)
            {
                for (UINT32 channel = 0; channel < _channels; ++channel)
                {
                    UINT32 offsetByte = _bytePerSample * (frame * _channels + channel);
                    std::memcpy(&workStation, pvSrc + offsetByte, _bytePerSample);
                    workStation = workStation * _currentGain;
                    std::memcpy(pvDst + offsetByte, &workStation, _bytePerSample);
                }

                _currentGain += param->Direction == FadeDirection::FORWARD ? _stepOfGain : -_stepOfGain;
                _currentGain = std::clamp(_currentGain, _minGain, _maxGain);
            }
            break;
        }

        case XAPO_BUFFER_SILENT: {
            // All that needs to be done for this case is setting the
            // output buffer flag to XAPO_BUFFER_SILENT which is done below.
            break;
        }
        }

        // set destination valid frame count, and buffer flags
        pOutputProcessParameters[0].ValidFrameCount =
            pInputProcessParameters[0].ValidFrameCount; // set destination frame count same as source
        pOutputProcessParameters[0].BufferFlags =
            pInputProcessParameters[0].BufferFlags; // set destination buffer flags same as source

        EndProcess();
    }
}