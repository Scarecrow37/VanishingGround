#include "pch.h"
#include "TimelineAudioEventContext.h"

REGISTER_CLASS(Timeline::EventTrack, Timeline::AudioEventContext)

namespace Timeline
{
    AudioEventContext::AudioEventContext() 
    {
        Path.SetInputAutoEvent([this]() {
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                {
                    DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                    File::Path           path      = data->GetPath();
                    const auto           extension = path.extension();
                    if (extension == L".wav")
                    {
                        SetAudioFromPath(path);
                    }
                }
                ImGui::EndDragDropTarget();
            }
        });
    }

    AudioEventContext::~AudioEventContext() 
    {
    }

    void AudioEventContext::OnNotify() 
    {
        UmAudio.Play(_audioGuid.string());
    }

    void AudioEventContext::SerializedReflectEvent() 
    {
        ReflectFields->AudioAssetID = UmFileSystem.GetAssetIDFromGuid(_audioGuid);
    }
    
    void AudioEventContext::DeserializedReflectEvent() 
    {
        _audioGuid = UmFileSystem.GetGuidFromAssetID(ReflectFields->AudioAssetID);
        UmAudio.LoadSound(_audioGuid.string(), _audioGuid);
    }

    void AudioEventContext::ImGuiDrawPropertysEvent() 
    {
    }

    void AudioEventContext::SetAudioFromGuid(const File::Guid& guid) 
    {
        _audioGuid = guid;
        ReflectFields->AudioAssetID = UmFileSystem.GetAssetIDFromGuid(_audioGuid);
    }

    void AudioEventContext::SetAudioFromPath(const File::Path& path) 
    {
        _audioGuid = path.ToGuid();
        ReflectFields->AudioAssetID = UmFileSystem.GetAssetIDFromGuid(_audioGuid);
    }
}
