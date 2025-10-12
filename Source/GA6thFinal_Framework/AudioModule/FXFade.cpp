#include "pch.h"
#include "FXFade.h"

#include <cassert>

namespace Audio
{
    FXFade::FXFade()
        : CXAPOParametersBase(&FX_FADE_REGISTRATION_PROPERTIES, reinterpret_cast<BYTE*>(_parameterBlocks),
                              sizeof(FadeParameter), FALSE),
          _isParameterChanged(0), _channels(0), _bytePerSample(0), _sampleRate(0), _minGain(0), _maxGain(0),
          _currentGain(0), _stepOfGain(0)
    {
    }

    HRESULT FXFade::LockForProcess(UINT32 InputLockedParameterCount,
                                   const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount,
                                   const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters)
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

        return CXAPOBase::LockForProcess(InputLockedParameterCount, pInputLockedParameters, OutputLockedParameterCount,
                                         pOutputLockedParameters);
    }

    void FXFade::Process(UINT32 InputProcessParameterCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled)
    {
        _isParameterChanged = ParametersChanged();

        BYTE* parameters = BeginProcess();

        assert(IsLocked());
        assert(InputProcessParameterCount == 1);
        assert(OutputProcessParameterCount == 1);
        assert(NULL != pInputProcessParameters);
        assert(NULL != pOutputProcessParameters);

        const FadeParameter* param = reinterpret_cast<FadeParameter*>(parameters);

        assert(param->BeginVolume >= FX_FADE_MIN_VOLUME && param->BeginVolume <= FX_FADE_MAX_VOLUME);
        assert(param->EndVolume >= FX_FADE_MIN_VOLUME && param->EndVolume <= FX_FADE_MAX_VOLUME);
        assert(param->Duration >= FX_FADE_MIN_DURATION);
        assert(param->Direction == FadeDirection::Forward || param->Direction == FadeDirection::Backward);
        assert(_sampleRate > 0);

        if (_isParameterChanged)
        {
            _currentGain = param->BeginVolume;

            const float fadeFrame = param->Duration * static_cast<float>(_sampleRate);
            _stepOfGain           = (param->EndVolume - param->BeginVolume) / fadeFrame;
            _minGain              = (std::min)(param->BeginVolume, param->EndVolume);
            _maxGain              = (std::max)(param->BeginVolume, param->EndVolume);
        }

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

            double workStation = 0.0;

            for (UINT32 frame = 0; frame < validFrameCount; ++frame)
            {
                for (UINT32 channel = 0; channel < _channels; ++channel)
                {
                    UINT32 offsetByte = (_bytePerSample * frame * _channels) + (_bytePerSample * _channels);
                    // TODO: 검증 필요
                    std::memcpy(&workStation, pvSrc + offsetByte, _bytePerSample);
                    workStation *= _currentGain;
                    std::memcpy(pvDst + offsetByte, &workStation, _bytePerSample);
                }

                _currentGain += param->Direction == FadeDirection::Forward ? _stepOfGain : -_stepOfGain;
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