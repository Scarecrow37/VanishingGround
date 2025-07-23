#include "pchScripts.h"
#include "AudioComponent.h"

AudioComponent::AudioComponent()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto context = data->pContext->lock(); nullptr != context)
                {
                    const auto& path = context->GetPath();
                    if (const auto extension = path.extension(); extension == L".wav")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        LoadAudio();
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void AudioComponent::Play()
{
    if (!_audioSource.has_value())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, reinterpret_cast<const char*>(u8"Audio Source가 없습니다."));
        return;
    }
    if (UmAudio.IsValidHandle(_audioHandle))
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING,
                     reinterpret_cast<const char*>(u8"이미 재생 중인 Audio Handle이 있습니다."));
        return;
    }
    _audioHandle = UmAudio.Play(_audioSource.value());
}

void AudioComponent::Stop()
{
    if (UmAudio.IsValidHandle(_audioHandle))
    {
        UmAudio.Stop(_audioHandle);
        _audioHandle = Audio::Handle();
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING,
                     reinterpret_cast<const char*>(u8"재생 중이지 않거나, 유효하지 않은 Audio Handle입니다."));
    }
}

void AudioComponent::DeserializedReflectEvent()
{
    Component::DeserializedReflectEvent();
    const File::Guid guid = ReflectFields->Guid;
    _guidRef        = guid;
    LoadAudio();
}

void AudioComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Play"))
    {
        Play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        Stop();
    }
}

void AudioComponent::LoadAudio()
{
    if (_guidRef != File::NULL_PATH)
    {
        _audioSource = UmAudio.CreateSoundFromWave(_guidRef.ToPath());
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, reinterpret_cast<const char*>(u8"Audio Guid가 유효하지 않습니다."));
    }
}