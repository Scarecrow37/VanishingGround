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
        AssetID.SetInputAutoEvent([this]() {
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
        UmAudio.Play(_guidStr);
    }

    void AudioEventContext::SerializedReflectEvent() 
    {
    }
    
    void AudioEventContext::DeserializedReflectEvent() 
    {
        _guid = UmFileSystem.GetGuidFromAssetID(ReflectFields->AudioAssetID);
        _guidStr = _guid.string();
        UmAudio.LoadSound(_guidStr, _guid);
    }

    void AudioEventContext::ImGuiDrawPropertysEvent() 
    {
    }

    void AudioEventContext::SetAudioFromGuid(const File::Guid& guid) 
    {
        _guid = guid;
        _guidStr = _guid.string();
        ReflectFields->AudioAssetID = UmFileSystem.GetAssetIDFromGuid(guid);
    }

    void AudioEventContext::SetAudioFromPath(const File::Path& path) 
    {
        _guid = path.ToGuid();
        _guidStr = _guid.string();
        ReflectFields->AudioAssetID = UmFileSystem.GetAssetIDFromGuid(_guid);
    }
}
