#include "pch.h"
#include "AudioManager.h"

void Audio::Manager::Initialize()
{
    _system.Initialize(IS_DEBUG);
    if constexpr (IS_DEBUG)
    {
        _system.TurnOnDebugMode();
    }

    for (unsigned char group = 0; group < GROUP_MAX; ++group)
    {
        _groups.emplace(static_cast<Group>(group), _system.CreateGroup());
    }

    _reverbHandle = _system.CreateReverbEffect(2, 44100);
    _fadeHandle   = _system.CreateFadeEffect(FadeInitParameter{0.2f, 1.0f,  3.0f}, 2, 44100);

    for (auto& groupHandle : _groups | std::views::values)
    {
        _system.EnableEffect(_fadeHandle, groupHandle);
    }
}

void Audio::Manager::Finalize()
{
    if constexpr (IS_DEBUG)
    {
        _system.TurnOffDebugMode();
    }
    _system.Finalize();
}

void Audio::Manager::ClearVoicePool()
{
    _system.ClearVoicePool();
}

void Audio::Manager::LoadSound(const std::string& key, const File::GuidRef& guid)
{
    const File::Path& path = guid.ToPath();
    if (false == path.IsNull())
    {
        Source source = _system.CreateSoundFromWave(path);
        if (const auto [iterator, isSucceed] = _sources.try_emplace(key, std::move(source)); false == isSucceed)
        {
            const std::string errorMsg = std::format("Audio Source with key '{}' already Loaded.", key);
            UmLogger.Log(LogLevel::LEVEL_INFO, errorMsg);
        }
    }
}

Audio::AudioHandle Audio::Manager::Play(const std::string& key, const Group group, const bool isLoop)
{
    if (!key.empty())
    {
        try
        {
            const auto& sound       = _sources.at(key);
            auto& groupHandle = _groups.at(group);

            return _system.Play(sound, std::span(&groupHandle, 1), isLoop);
        }
        catch (const std::out_of_range& exception)
        {
            const std::string errorMessage = std::format("{} : Audio source does not exist.", key);
            UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
            UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
        }
        catch (const AudioException& exception)
        {
            const std::string errorMessage = std::format("{} : Audio Error when play sound.", key);
            UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
            UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
        }
        catch (const std::exception& exception)
        {
            const std::string errorMessage = std::format("{} : Unknown Error when play sound.", key);
            UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
            UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Empty Key.");
    }
    return AudioHandle{};
}

void Audio::Manager::Stop(const AudioHandle& handle)
{
    if (_system.IsValidHandle(handle))
    {
        try
        {
            _system.Stop(handle);
        }
        catch (const AudioException& exception)
        {
            const std::string errorMessage = std::format("Audio Error when stop sound.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
            UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
        }
        catch (const std::exception& exception)
        {
            const std::string errorMessage = std::format("Unknown Error when stop sound.");
            UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
            UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
        }
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_INFO, "Invalid handle.");
    }
}

void Audio::Manager::FadeIn() const
{
    _system.SetEffectParameter(_fadeHandle, FadeParameter{FadeDirection::FORWARD});
}

void Audio::Manager::FadeOut() const
{
    _system.SetEffectParameter(_fadeHandle, FadeParameter{FadeDirection::BACKWARD});
}