#pragma once

namespace Audio
{
    using WaveFormatHash                              = unsigned long long;
    constexpr WaveFormatHash INVALID_WAVE_FORMAT_HASH = 0;

    using Index                   = long long;
    constexpr Index INVALID_INDEX = -1;

    using Generation                        = long long;
    constexpr Generation INVALID_GENERATION = -1;

    struct IncreaseGeneration
    {
        Generation& operator()(Generation& generation) const;
    };

    struct IsUnusedGeneration
    {
        bool operator()(const Generation& generation) const;
    };

    constexpr UINT32 PROCESSING_STAGE_GROUP = 0;
    constexpr UINT32 PROCESSING_STAGE_EFFECT = 1;

    enum class EffectType : unsigned char
    {
        REVERB,
        FADE,
    };

    constexpr float FX_REVERB_MIN_DIFFUSION     = FXREVERB_MIN_DIFFUSION;
    constexpr float FX_REVERB_MAX_DIFFUSION     = FXREVERB_MAX_DIFFUSION;
    constexpr float FX_REVERB_MIN_ROOM_SIZE     = FXREVERB_MIN_ROOMSIZE;
    constexpr float FX_REVERB_MAX_ROOM_SIZE     = FXREVERB_MAX_ROOMSIZE;
    constexpr float FX_REVERB_DEFAULT_DIFFUSION = FXREVERB_DEFAULT_DIFFUSION;
    constexpr float FX_REVERB_DEFAULT_ROOM_SIZE = FXREVERB_DEFAULT_ROOMSIZE;

    struct ReverbParameter
    {
        ReverbParameter() : Diffusion(FX_REVERB_DEFAULT_DIFFUSION), RoomSize(FX_REVERB_DEFAULT_ROOM_SIZE) {}
        ReverbParameter(const float diffusion, const float roomSize) : Diffusion(diffusion), RoomSize(roomSize) {}

        /// <summary>
        /// 확산 정도입니다.
        /// 범위는 0.0f ~ 1.0f 입니다.
        /// 기본 값은 0.9f 입니다.
        /// </summary>
        float Diffusion;

        /// <summary>
        /// 반향이 생기는 공간의 크기입니다.
        /// 범위는 0.0001f ~ 1.0f 입니다.
        /// 기본 값은 0.6f 입니다.
        /// </summary>
        float RoomSize;
    };

    enum class FadeDirection : unsigned char
    {
        FORWARD,
        BACKWARD
    };

    constexpr float         FX_FADE_MIN_VOLUME           = 0.0f;
    constexpr float         FX_FADE_MAX_VOLUME           = 1.0f;
    constexpr float         FX_FADE_DEFAULT_BEGIN_VOLUME = 0.0f;
    constexpr float         FX_FADE_DEFAULT_END_VOLUME   = 1.0f;
    constexpr float         FX_FADE_MIN_DURATION         = 0.0001f;
    constexpr float         FX_FADE_DEFAULT_DURATION     = 1.0f;
    constexpr FadeDirection FX_FADE_DEFAULT_DIRECTION    = FadeDirection::FORWARD;

    struct FadeInitParameter
    {
        FadeInitParameter()
            : BeginVolume(FX_FADE_DEFAULT_BEGIN_VOLUME), EndVolume(FX_FADE_DEFAULT_END_VOLUME),
              Duration(FX_FADE_DEFAULT_DURATION)
        {
        }

        FadeInitParameter(const float beginVolume, const float endVolume, const float duration)
            : BeginVolume(beginVolume), EndVolume(endVolume), Duration(duration)
        {
        }

        /// <summary>
        /// 시작 볼륨입니다.
        /// 범위는 0.0f ~ 1.0f 입니다.
        /// 기본 값은 0.0f 입니다.
        /// </summary>
        float BeginVolume;

        /// <summary>
        /// 끝 볼륨입니다.
        /// 범위는 0.0f ~ 1.0f 입니다.
        /// 기본 값은 1.0f 입니다.
        /// </summary>
        float EndVolume;

        /// <summary>
        /// 효과를 유지할 시간(초)입니다.
        /// 범위는 0.0001f 이상입니다.
        /// 기본 값은 1.0f 입니다.
        /// </summary>
        float Duration;
    };

    struct FadeParameter
    {
        FadeParameter()
            : Direction(FX_FADE_DEFAULT_DIRECTION)
        {
        }

        explicit FadeParameter(const FadeDirection direction) : Direction(direction) {}

        /// <summary>
        /// 효과를 적용할 방향입니다.
        /// Forward는 BeginVolume에서 EndVolume으로, Backward는 EndVolume에서 BeginVolume으로 변화합니다.
        /// 기본 값은 Forward입니다.
        /// </summary>
        FadeDirection Direction;
    };
} // namespace Audio