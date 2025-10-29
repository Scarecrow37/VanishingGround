#include "pch.h"
#include "System.h"

#include "FXFade.h"

#include <ranges>

namespace Audio
{
    namespace
    {
#ifdef _XBOX
        constexpr FOURCC RIFF = MAKEFOURCC('F', 'F', 'I', 'R');
        constexpr FOURCC DATA = MAKEFOURCC('a', 't', 'a', 'd');
        constexpr FOURCC FMT  = MAKEFOURCC(' ', 't', 'm', 'f');
        constexpr FOURCC WAVE = MAKEFOURCC('E', 'V', 'A', 'W');
        constexpr FOURCC XWMA = MAKEFOURCC('A', 'M', 'W', 'X');
        constexpr FOURCC DPDS = MAKEFOURCC('s', 'd', 'p', 'd');
#else
        constexpr FOURCC RIFF = MAKEFOURCC('R', 'I', 'F', 'F');
        constexpr FOURCC DATA = MAKEFOURCC('d', 'a', 't', 'a');
        constexpr FOURCC FMT  = MAKEFOURCC('f', 'm', 't', ' ');
        constexpr FOURCC WAVE = MAKEFOURCC('W', 'A', 'V', 'E');
        constexpr FOURCC XWMA = MAKEFOURCC('X', 'W', 'M', 'A');
        constexpr FOURCC DPDS = MAKEFOURCC('D', 'P', 'D', 'S');
#endif

        struct FindChunk
        {
            std::pair<std::streamsize, std::streamoff> operator()(std::ifstream& fileStream, const FOURCC fourcc) const
            {
                static constexpr std::streamsize FOURCC_COUNT          = sizeof(FOURCC);
                static constexpr std::streamsize CHUNK_DATA_SIZE_COUNT = sizeof(DWORD);

                // 초기화
                FOURCC format;

                std::streamsize chunkDataSize = 0;
                std::streamoff  offset        = 0;

                // 파일 포인터를 파일의 시작으로 설정.
                fileStream.seekg(0, std::ios::beg);

                while (!fileStream.eof())
                {
                    fileStream.read(reinterpret_cast<char*>(&format), FOURCC_COUNT);
                    fileStream.read(reinterpret_cast<char*>(&chunkDataSize), CHUNK_DATA_SIZE_COUNT);

                    if (format == RIFF)
                    {
                        chunkDataSize = FOURCC_COUNT;
                        fileStream.seekg(FOURCC_COUNT, std::ios::cur);
                    }
                    else
                    {
                        fileStream.seekg(chunkDataSize, std::ios::cur);
                    }

                    offset += FOURCC_COUNT + CHUNK_DATA_SIZE_COUNT;

                    if (format == fourcc)
                    {
                        return {chunkDataSize, offset};
                    }

                    offset += chunkDataSize;
                }
                throw AudioException("Chunk not found: " + std::to_string(fourcc));
            }
        };

        struct ReadChunkData
        {
            void operator()(std::ifstream& fileStream, void* buffer, const std::streamsize& bufferSize,
                            const std::streamoff bufferOffset) const
            {
                fileStream.seekg(bufferOffset, std::ios::beg);
                fileStream.read(static_cast<char*>(buffer), bufferSize);
                if (fileStream.gcount() != bufferSize)
                    throw AudioException("Failed to read chunk data: " + std::to_string(bufferSize));
            }
        };

        struct ThrowIfFailed
        {
            void operator()(const HRESULT result, const std::string& message) const
            {
                if (FAILED(result))
                {
                    switch (result)
                    {
                    case XAUDIO2_E_INVALID_CALL:
                        throw InvalidCallException(message);
                    case XAUDIO2_E_XMA_DECODER_ERROR:
                        throw XmaDecoderException(message);
                    case XAUDIO2_E_XAPO_CREATION_FAILED:
                        throw EffectCreationException(message);
                    case XAUDIO2_E_DEVICE_INVALIDATED:
                        throw DeviceInvalidatedException(message);
                    default:
                        LPSTR       buffer;
                        const DWORD count = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                                               FORMAT_MESSAGE_IGNORE_INSERTS,
                                                           nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                                           reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
                        std::string fullMessage = message;
                        if (buffer != nullptr)
                        {
                            fullMessage.append(buffer);
                            LocalFree(buffer);
                        }
                        throw AudioException(fullMessage);
                    }
                }
            }
        };

        struct GetWaveFormatHash
        {
            template <typename T>
            void Combine(WaveFormatHash& seed, T value) const
            {
                seed ^= std::hash<T>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            WaveFormatHash operator()(const WAVEFORMATEXTENSIBLE& waveFormat) const
            {
                WaveFormatHash seed = 0;
                Combine(seed, waveFormat.Format.wFormatTag);
                Combine(seed, waveFormat.Format.nChannels);
                Combine(seed, waveFormat.Format.nSamplesPerSec);
                Combine(seed, waveFormat.Format.nAvgBytesPerSec);
                Combine(seed, waveFormat.Format.nBlockAlign);
                Combine(seed, waveFormat.Format.wBitsPerSample);
                Combine(seed, waveFormat.Format.cbSize);
                Combine(seed, waveFormat.Samples.wValidBitsPerSample);
                Combine(seed, waveFormat.dwChannelMask);
                Combine(seed, waveFormat.SubFormat.Data1);
                Combine(seed, waveFormat.SubFormat.Data2);
                Combine(seed, waveFormat.SubFormat.Data3);
                unsigned long long data4;
                std::memcpy(&data4, waveFormat.SubFormat.Data4, sizeof(data4));
                Combine(seed, data4);
                return seed;
            }
        };
    } // namespace

    System::System() = default;

    System::System(System&& other) noexcept
        : _xAudio2(std::move(other._xAudio2)), _masteringVoice(other._masteringVoice)
    {
        other._masteringVoice = nullptr;
    }

    System& System::operator=(System&& other) noexcept
    {
        if (this == &other)
            return *this;
        _xAudio2              = std::move(other._xAudio2);
        _masteringVoice       = other._masteringVoice;
        other._masteringVoice = nullptr;
        return *this;
    }

    System::~System() = default;

    void System::Initialize(const bool isDebug)
    {
        constexpr ThrowIfFailed throwIfFailed;

        throwIfFailed(XAudio2Create(&_xAudio2, isDebug ? XAUDIO2_DEBUG_ENGINE : 0, XAUDIO2_DEFAULT_PROCESSOR),
                      "Failed to create XAudio2 instance");

        throwIfFailed(_xAudio2->CreateMasteringVoice(&_masteringVoice), "Failed to create mastering voice");
    }

    void System::Finalize()
    {
        ClearVoicePool();

        ClearGroupPool();

        ClearEffectPool();

        if (_masteringVoice)
        {
            _masteringVoice->DestroyVoice();
            _masteringVoice = nullptr;
        }

        if (_xAudio2)
        {
            _xAudio2->Release();
            _xAudio2 = nullptr;
        }
    }

    void System::ClearVoicePool()
    {
        for (auto& voiceVector : _voicePools | std::views::values)
        {
            for (auto& sourceVoice : voiceVector)
            {
                if (sourceVoice.Voice)
                {
                    sourceVoice.Voice->DestroyVoice();
                    sourceVoice.Voice = nullptr;
                }
            }
            voiceVector.clear();
        }
        _voicePools.clear();
    }

    void System::ClearGroupPool()
    {
        for (auto& submixVoice : _groupPool)
        {
            if (submixVoice.Voice)
            {
                submixVoice.Voice->DestroyVoice();
                submixVoice.Voice = nullptr;
            }
        }
        _groupPool.clear();
    }

    void System::ClearEffectPool()
    {
        for (auto& effectVoice : _effectPool)
        {
            if (effectVoice.Voice)
            {
                effectVoice.Voice->DestroyVoice();
                effectVoice.Voice = nullptr;
            }
        }
        _effectPool.clear();
    }

    void System::TurnOnDebugMode() const
    {
        XAUDIO2_DEBUG_CONFIGURATION debugConfig{};
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_INFO | XAUDIO2_LOG_DETAIL |
                                XAUDIO2_LOG_API_CALLS | XAUDIO2_LOG_FUNC_CALLS;
        _xAudio2->SetDebugConfiguration(&debugConfig);
    }

    void System::TurnOffDebugMode() const
    {
        constexpr XAUDIO2_DEBUG_CONFIGURATION debugConfig{};
        _xAudio2->SetDebugConfiguration(&debugConfig);
    }

    Source System::CreateSoundFromWave(const std::filesystem::path& filePath)
    {
        std::ifstream fileStream;
        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fileStream.open(filePath, std::ios::binary);

        constexpr FindChunk     findChunk;
        constexpr ReadChunkData readChunkData;

        // RIFF 청크
        auto [riffSize, riffPosition] = findChunk(fileStream, RIFF);

        FOURCC fileType;
        readChunkData(fileStream, &fileType, riffSize, riffPosition);

        if (fileType != WAVE)
            throw AudioException("Invalid file format: Not a WAVE file");

        // Format 청크
        auto [fmtSize, fmtPosition] = findChunk(fileStream, FMT);

        WAVEFORMATEXTENSIBLE wfx{};
        readChunkData(fileStream, &wfx, fmtSize, fmtPosition);

        // Data 청크
        auto [dataSize, dataPosition] = findChunk(fileStream, DATA);

        BYTE* audioData = new BYTE[dataSize];
        try
        {
            readChunkData(fileStream, audioData, dataSize, dataPosition);
        }
        catch (...)
        {
            delete[] audioData;
            throw;
        }

        return Source{wfx, audioData, static_cast<UINT32>(dataSize)};
    }

    AudioHandle System::Play(const Source& sound, std::span<GroupHandle> groups, const bool isLoop)
    {
        if (_xAudio2 == nullptr)
            throw AudioException("Audio manager is not initialized.");

        constexpr IncreaseGeneration increaseGeneration;

        // Format 해싱
        const WaveFormatHash hash = GetWaveFormatHash()(sound._format);

        // Format에 맞는 Pool 찾기
        auto [iter, succeed] = _voicePools.try_emplace(hash, VoicePool{});

        if (succeed)
        {
            iter->second.resize(
                MAX_POOL_SIZE, SourceVoice{.Generation = 0, .Callback = Callback(OnBufferEnd(this)), .Voice = nullptr});
        }

        VoicePool& sourceVoices = iter->second;

        // 미사용 Voice 찾기
        auto unusedSourceVoiceIterator = std::ranges::find_if(sourceVoices, [](const SourceVoice& sv) {
            static IsUnusedGeneration isUnusedGeneration;
            return isUnusedGeneration(sv.Generation);
        });

        if (unusedSourceVoiceIterator == sourceVoices.end())
        {
            throw AudioException("No available source voices in the pool.");
        }

        auto& [unusedSourceVoiceGeneration, unusedSourceVoiceCallback, unusedSourceVoice] = *unusedSourceVoiceIterator;

        increaseGeneration(unusedSourceVoiceGeneration);
        const Index index = std::distance(sourceVoices.begin(), unusedSourceVoiceIterator);

        // AudioHandle 생성
        const AudioHandle handle = {hash, index, unusedSourceVoiceGeneration};
        // Callback 갱신
        unusedSourceVoiceCallback.SetHandle(handle);

        try
        {
            constexpr ThrowIfFailed throwIfFailed;

            // 필요시 IXAudio2SourceVoice 생성
            if (nullptr == unusedSourceVoice)
            {
                throwIfFailed(_xAudio2->CreateSourceVoice(&unusedSourceVoice,
                                                          reinterpret_cast<const WAVEFORMATEX*>(&sound._format), NULL,
                                                          XAUDIO2_DEFAULT_FREQ_RATIO, &unusedSourceVoiceCallback),
                              "Failed to create source voice.");
            }

            // XAUDIO2_BUFFER 설정
            XAUDIO2_BUFFER buffer{};
            buffer.Flags      = NULL;
            buffer.AudioBytes = sound._bytes;
            buffer.pAudioData = sound._buffer;
            buffer.PlayBegin  = 0;
            buffer.PlayLength = 0; // 0이면 전체 재생
            buffer.LoopBegin  = 0;
            buffer.LoopLength = 0; // 0이면 전체 루프
            buffer.LoopCount  = isLoop ? XAUDIO2_LOOP_INFINITE : XAUDIO2_NO_LOOP_REGION;
            buffer.pContext   = nullptr;

            // Submit 후 시작
            throwIfFailed(unusedSourceVoice->SubmitSourceBuffer(&buffer), "Failed to submit source buffer.");

            // Group 설정
            if (groups.empty() == false)
            {
                std::vector<XAUDIO2_SEND_DESCRIPTOR> sendDescriptors;
                sendDescriptors.reserve(groups.size());

                const GroupHandle& firstGroupHandle = groups.front();
                if (false == IsValidHandle(firstGroupHandle))
                {
                    throw InvalidHandleException("Invalid group handle provided.");
                }

                const GroupVoice& firstSubmixVoice = _groupPool.at(firstGroupHandle._index);

                const UINT32 channel    = firstSubmixVoice.Channels;
                const UINT32 sampleRate = firstSubmixVoice.SampleRate;

                std::unordered_set<Index> uniqueIndices;
                for (auto& group : groups)
                {
                    if (auto [_, isUnique] = uniqueIndices.emplace(group._index); false == isUnique)
                    {
                        throw AudioException("Duplicate group handles are not allowed.");
                    }

                    if (false == IsValidHandle(group))
                    {
                        throw InvalidHandleException("Invalid group handle provided.");
                    }

                    GroupVoice& submixVoice = _groupPool.at(group._index);
                    if (submixVoice.Channels != channel || submixVoice.SampleRate != sampleRate)
                    {
                        throw AudioException("All groups must have the same channel count and sample rate.");
                    }

                    sendDescriptors.emplace_back(NULL, submixVoice.Voice);

                    std::erase_if(submixVoice.AttachedVoices,
                                  [this](const AudioHandle& attachedVoice) { return !IsValidHandle(attachedVoice); });
                    submixVoice.AttachedVoices.push_back(handle);
                }
                const XAUDIO2_VOICE_SENDS voiceSends{.SendCount = static_cast<UINT32>(groups.size()),
                                                     .pSends    = sendDescriptors.data()};

                throwIfFailed(unusedSourceVoice->SetOutputVoices(&voiceSends), "Failed to set output voices.");
            }

            throwIfFailed(unusedSourceVoice->Start(0), "Failed to start source voice.");
        }
        catch (...)
        {
            increaseGeneration(unusedSourceVoiceGeneration);
            throw;
        }

        return handle;
    }

    void System::Stop(const AudioHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to Stop.");

        constexpr ThrowIfFailed throwIfFailed;

        const SourceVoice& sourceVoice = _voicePools.at(handle._hash).at(handle._index);
        throwIfFailed(sourceVoice.Voice->Stop(0), "Failed to stop source voice.");

        ReleaseVoice(handle);
    }

    bool System::IsValidHandle(const AudioHandle& handle) const noexcept
    {
        if (_voicePools.contains(handle._hash))
        {
            const VoicePool& voicePool = _voicePools.at(handle._hash);
            const bool       isValidIndex =
                handle._index >= 0 && static_cast<decltype(voicePool.size())>(handle._index) < voicePool.size();
            if (isValidIndex)
            {
                const SourceVoice& sourceVoice = voicePool.at(handle._index);
                return handle._generation == sourceVoice.Generation;
            }
        }
        return false;
    }

    bool System::IsValidHandle(const GroupHandle& handle) const noexcept
    {
        if (handle._index >= 0 && static_cast<decltype(_groupPool.size())>(handle._index) < _groupPool.size())
        {
            const GroupVoice& group = _groupPool.at(handle._index);
            return handle._generation == group.Generation;
        }
        return false;
    }

    bool System::IsValidHandle(const EffectHandle& handle) const noexcept
    {
        if (handle._index >= 0 && static_cast<decltype(_effectPool.size())>(handle._index) < _effectPool.size())
        {
            const EffectVoice& effect = _effectPool.at(handle._index);
            return handle._generation == effect.Generation;
        }
        return false;
    }

    GroupHandle System::CreateGroup(const UINT32 channels, const UINT32 sampleRate)
    {
        constexpr ThrowIfFailed      throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        for (size_t i = 0; i < _groupPool.size(); ++i)
        {
            constexpr IsUnusedGeneration isUnusedGeneration;

            if (auto& groupVoice = _groupPool[i]; isUnusedGeneration(groupVoice.Generation) &&
                                                  groupVoice.Voice != nullptr && groupVoice.Channels == channels &&
                                                  groupVoice.SampleRate == sampleRate)
            {
                increaseGeneration(groupVoice.Generation);
                return GroupHandle{static_cast<Index>(i), groupVoice.Generation};
            }
        }

        GroupVoice& newSubmixVoice =
            _groupPool.emplace_back(0, channels, sampleRate, nullptr, std::list<AudioHandle>());

        throwIfFailed(_xAudio2->CreateSubmixVoice(&newSubmixVoice.Voice, channels, sampleRate, NULL, PROCESSING_STAGE_GROUP),
                      "Failed to create submix voice.");

        const Generation generation = increaseGeneration(newSubmixVoice.Generation);
        const Index      index      = static_cast<Index>(_groupPool.size() - 1);

        return GroupHandle{index, generation};
    }

    void System::ReleaseGroup(const GroupHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid group handle provided to ReleaseGroup.");

        auto& [generation, channels, sampleRate, voice, attachedVoices] = _groupPool.at(handle._index);
        IncreaseGeneration()(generation);
        std::ranges::for_each(attachedVoices, [this](const AudioHandle& attachedVoice) {
            if (IsValidHandle(attachedVoice))
                DetachOutput(attachedVoice);
        });
        attachedVoices.clear();
    }

    void System::SetVolume(const float volume) const
    {
        const float newVolume = std::clamp(volume, 0.0f, 1.0f);
        ThrowIfFailed()(_masteringVoice->SetVolume(newVolume), "Failed to set master volume.");
    }

    void System::SetVolume(const GroupHandle& handle, const float volume) const
    {
        if (IsValidHandle(handle))
        {
            const float newVolume = std::clamp(volume, 0.0f, 1.0f);
            ThrowIfFailed()(_groupPool.at(handle._index).Voice->SetVolume(newVolume), "Failed to set group volume.");
        }
    }

    void System::SetVolume(const AudioHandle& handle, const float volume) const
    {
        if (IsValidHandle(handle))
        {
            const float newVolume = std::clamp(volume, 0.0f, 1.0f);
            ThrowIfFailed()(_voicePools.at(handle._hash).at(handle._index).Voice->SetVolume(newVolume),
                "Failed to set source voice volume.");
        }
    }

    ReverbHandle System::CreateReverbEffect(UINT32 channels, UINT32 sampleRate)
    {
        constexpr ThrowIfFailed      throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        for (size_t i = 0; i < _effectPool.size(); ++i)
        {
            constexpr IsUnusedGeneration isUnusedGeneration;

            if (auto& effectVoice = _effectPool[i]; isUnusedGeneration(effectVoice.Generation) &&
                                                    effectVoice.Type == EffectType::REVERB &&
                                                    effectVoice.Voice != nullptr && effectVoice.Channels == channels &&
                                                    effectVoice.SampleRate == sampleRate)
            {
                increaseGeneration(effectVoice.Generation);

                return ReverbHandle{static_cast<Index>(i), effectVoice.Generation};
            }
        }

        EffectVoice& newEffectVoice =
            _effectPool.emplace_back(EffectType::REVERB, 0, channels, sampleRate, nullptr, std::list<GroupHandle>());

        IUnknown* effect = nullptr;
        throwIfFailed(CreateFX(__uuidof(FXReverb), &effect), "Failed to create effect.");

        XAUDIO2_EFFECT_DESCRIPTOR effectDescriptor{.pEffect = effect, .InitialState = FALSE, .OutputChannels = channels};

        const XAUDIO2_EFFECT_CHAIN effectChain{.EffectCount = 1, .pEffectDescriptors = &effectDescriptor};

        throwIfFailed(_xAudio2->CreateSubmixVoice(&newEffectVoice.Voice, channels, sampleRate, NULL, PROCESSING_STAGE_EFFECT, nullptr, &effectChain),
                      "Failed to create submix voice.");

        if (effect)
            effect->Release();

        const Generation generation = increaseGeneration(newEffectVoice.Generation);
        const Index      index      = static_cast<Index>(_effectPool.size() - 1);

        return ReverbHandle{index, generation};
    }

    void System::SetEffectParameter(const ReverbHandle& handle, const ReverbParameter parameter) const
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid effect handle provided to SetEffectParameter.");

        constexpr ThrowIfFailed throwIfFailed;

        const EffectVoice& effectVoice = _effectPool.at(handle._index);

        if (effectVoice.Type != EffectType::REVERB)
            throw InvalidCallException("Effect handle is not of type Reverb.");

        FXREVERB_PARAMETERS reverbParameters;
        reverbParameters.Diffusion = parameter.Diffusion;
        reverbParameters.RoomSize  = parameter.RoomSize;

        throwIfFailed(effectVoice.Voice->SetEffectParameters(0, &reverbParameters, sizeof(FXREVERB_PARAMETERS)),
                      "Failed to set reverb parameters.");
    }

    FadeHandle System::CreateFadeEffect(const FadeInitParameter parameter, UINT32 channels, UINT32 sampleRate)
    {
        constexpr ThrowIfFailed      throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        for (size_t i = 0; i < _effectPool.size(); ++i)
        {
            constexpr IsUnusedGeneration isUnusedGeneration;

            if (auto& effectVoice = _effectPool[i]; isUnusedGeneration(effectVoice.Generation) &&
                                                    effectVoice.Type == EffectType::FADE &&
                                                    effectVoice.Voice != nullptr && effectVoice.Channels == channels &&
                                                    effectVoice.SampleRate == sampleRate)
            {
                increaseGeneration(effectVoice.Generation);

                return FadeHandle{static_cast<Index>(i), effectVoice.Generation};
            }
        }

        EffectVoice& newEffectVoice =
            _effectPool.emplace_back(EffectType::FADE, 0, channels, sampleRate, nullptr, std::list<GroupHandle>());

        throwIfFailed(_xAudio2->CreateSubmixVoice(&newEffectVoice.Voice, channels, sampleRate, NULL, PROCESSING_STAGE_EFFECT),
                      "Failed to create submix voice.");

        IXAPO* effect = new FXFade(parameter);

        XAUDIO2_EFFECT_DESCRIPTOR effectDescriptor{.pEffect = effect, .InitialState = FALSE, .OutputChannels = channels};

        const XAUDIO2_EFFECT_CHAIN effectChain{.EffectCount = 1, .pEffectDescriptors = &effectDescriptor};

        throwIfFailed(newEffectVoice.Voice->SetEffectChain(&effectChain), "Failed to set effect chain.");

        if (effect)
            effect->Release();

        const Generation generation = increaseGeneration(newEffectVoice.Generation);
        const Index      index      = static_cast<Index>(_effectPool.size() - 1);

        return FadeHandle{index, generation};
    }

    void System::SetEffectParameter(const FadeHandle& handle, const FadeParameter parameter) const
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid effect handle provided to SetEffectParameter.");

        constexpr ThrowIfFailed throwIfFailed;

        const EffectVoice& effectVoice = _effectPool.at(handle._index);

        if (effectVoice.Type != EffectType::FADE)
            throw InvalidCallException("Effect handle is not of type Fade.");

        throwIfFailed(effectVoice.Voice->SetEffectParameters(0, &parameter, sizeof(FadeParameter)),
                      "Failed to set fade parameters.");
    }

    void System::ReleaseEffect(const EffectHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid group handle provided to ReleaseGroup.");

        auto& [type, generation, channels, sampleRate, voice, attachedVoices] = _effectPool.at(handle._index);
        IncreaseGeneration()(generation);
        std::ranges::for_each(attachedVoices, [this](const GroupHandle& attachedVoice) {
            if (IsValidHandle(attachedVoice))
                DetachOutput(attachedVoice);
        });
        attachedVoices.clear();
    }

    void System::AttachEffect(const EffectHandle& effectHandle, const GroupHandle& groupHandle)
    {
        if (!IsValidHandle(effectHandle))
            throw InvalidHandleException("Invalid effect effectHandle provided to EnableEffect.");

        if (!IsValidHandle(groupHandle))
            throw InvalidHandleException("Invalid group handle provided to EnableEffect.");

        EffectVoice&      effectVoice = _effectPool.at(effectHandle._index);
        const GroupVoice& groupVoice  = _groupPool.at(groupHandle._index);

        if (const auto findIter = std::ranges::find(effectVoice.AttachedVoices, groupHandle);
            findIter != effectVoice.AttachedVoices.end())
            return;

        constexpr ThrowIfFailed throwIfFailed;

        XAUDIO2_SEND_DESCRIPTOR   sendDescriptor{.Flags = NULL, .pOutputVoice = effectVoice.Voice};
        const XAUDIO2_VOICE_SENDS voiceSends{.SendCount = 1, .pSends = &sendDescriptor};

        throwIfFailed(groupVoice.Voice->SetOutputVoices(&voiceSends), "Failed to set output voices.");

        effectVoice.AttachedVoices.push_back(groupHandle);
    }

    void System::DetachEffect(const EffectHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid effect handle provided to DisableEffect.");

        constexpr ThrowIfFailed throwIfFailed;

        EffectVoice& effectVoice = _effectPool.at(handle._index);

        for (auto& groupHandle : effectVoice.AttachedVoices)
        {
            if (IsValidHandle(groupHandle))
                DetachOutput(groupHandle);
        }

        effectVoice.AttachedVoices.clear();
    }

    void System::EnableEffect(const EffectHandle& handle) const
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid effect handle provided to EnableEffect.");

        constexpr ThrowIfFailed throwIfFailed;

        const EffectVoice& effectVoice = _effectPool.at(handle._index);
        throwIfFailed(effectVoice.Voice->EnableEffect(0), "Failed to enable effect.");
    }

    void System::DisableEffect(const EffectHandle& handle) const
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid effect handle provided to DisableEffect.");

        constexpr ThrowIfFailed throwIfFailed;

        const EffectVoice& effectVoice = _effectPool.at(handle._index);
        throwIfFailed(effectVoice.Voice->DisableEffect(0), "Failed to disable effect.");
    }

    void System::ReleaseVoice(const AudioHandle& handle)
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to ReleaseVoice.");

        constexpr ThrowIfFailed      throwIfFailed;
        constexpr IncreaseGeneration increaseGeneration;

        auto& [generation, callback, voice] = _voicePools.at(handle._hash).at(handle._index);
        throwIfFailed(voice->FlushSourceBuffers(), "Failed to flush source buffers.");
        callback.SetHandle(AudioHandle());
        increaseGeneration(generation);
    }

    void System::DetachOutput(const AudioHandle& handle) const
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid handle provided to DetachOutput.");

        if (IXAudio2SourceVoice* voice = _voicePools.at(handle._hash).at(handle._index).Voice; nullptr != voice)
        {
            ThrowIfFailed()(voice->SetOutputVoices(nullptr), "Failed to detach output voices.");
        }
    }

    void System::DetachOutput(const GroupHandle& handle) const
    {
        if (!IsValidHandle(handle))
            throw InvalidHandleException("Invalid group handle provided to DetachOutput.");
        if (IXAudio2SubmixVoice* voice = _groupPool.at(handle._index).Voice; nullptr != voice)
        {
            ThrowIfFailed()(voice->SetOutputVoices(nullptr), "Failed to detach output voices.");
        }
    }

    System::OnBufferEnd::OnBufferEnd(Audio::System* system) : System(system) {}

    void System::OnBufferEnd::operator()(const AudioHandle& handle) const
    {
        if (nullptr != System && System->IsValidHandle(handle))
        {
            System->ReleaseVoice(handle);
        }
    }

} // namespace Audio