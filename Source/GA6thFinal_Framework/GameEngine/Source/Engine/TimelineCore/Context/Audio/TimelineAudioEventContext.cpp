#include "pch.h"
#include "TimelineAudioEventContext.h"

REGISTER_CLASS(Timeline::EventTrack, Timeline::AudioEventContext)

namespace Timeline
{
    AudioEventContext::AudioEventContext() = default;
    AudioEventContext::~AudioEventContext() = default;

    void AudioEventContext::OnNotify() 
    {
        if (ReflectFields->AudioDataTable.empty())
        {
            return;
        }

        float randomWeight = Random::Range(0.0f, _totalWeight);
        float currentWeight = 0.0f;
        for (auto& [assetID, weight] : ReflectFields->AudioDataTable)
        {
            currentWeight += weight;
            if (randomWeight <= currentWeight)
            {
                const File::Guid& guid = UmFileSystem.GetGuidFromAssetID(assetID);
                UmAudio.Play(guid.string());
                break;
            }
        }
    }

    void AudioEventContext::SerializedReflectEvent() 
    {
    }
    
    void AudioEventContext::DeserializedReflectEvent() 
    {
        for (auto& [assetID, weight] : ReflectFields->AudioDataTable)
        {
            _totalWeight += weight;
            const File::Guid& guid = UmFileSystem.GetGuidFromAssetID(assetID);
            if (false == guid.IsNull())
            {
                std::string guidStr = guid.string();
                _audioGuidTable[assetID] = guidStr;
                UmAudio.LoadSound(guidStr, guid);
            }
        }
    }

    void AudioEventContext::ImGuiDrawPropertysEvent() 
    {
        const char* mode = _isShowPath ? "Show AssetID" : "Show Path";
        ImGui::Checkbox(mode, &_isShowPath);

        if (ImGui::TreeNodeEx("AudioList", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const float height = ImGui::GetItemRectSize().y;

            if (ImGui::BeginTable("ContextTable", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
            {
                const char* first = _isShowPath ? "Path" : "AssetID";
                ImGui::TableSetupColumn(first, ImGuiTableColumnFlags_WidthStretch, 0.7f);
                ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                ImGui::TableHeadersRow();

                for (auto& [assetID, weight] : ReflectFields->AudioDataTable)
                {
                    const File::Guid& guid = UmFileSystem.GetGuidFromAssetID(assetID);
                    const File::Path& path = UmFileSystem.GetPathFromGuid(guid);

                    ImGui::PushID(assetID);
                    ImGui::TableNextRow();
                    {
                        std::string label = _isShowPath ? path.string() : std::to_string(assetID);
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(label.c_str());
                        ImGuiHelper::HoveredToolTip(label.c_str());
                    }

                    {
                        ImGui::TableSetColumnIndex(1);
                        ImVec2 availSize = ImGui::GetContentRegionAvail();
                        ImGui::SetNextItemWidth(availSize.x - 60.0f);
                        ImGui::DragFloat("##Weight", &weight, 0.01f, 0.0f, 100.0f);

                        ImGui::SameLine();
                        if (ImGui::Button("-", ImVec2(height, height)))
                        {
                            RemoveAudioFromAssetID(assetID);
                        }
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            const ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.0f);
            if (ImGui::InputInt("##new_file", &_newAssetID, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                AddAudioFromAssetID(_newAssetID);
                _newAssetID = 0;
            }
            ListenAudioFileDragDropEvent();
            ImGui::SameLine();
            if (ImGui::Button("+", ImVec2(height, height)))
            {
                AddAudioFromAssetID(_newAssetID);
                _newAssetID = 0;
            }

            ImGui::TreePop();
        }
    }

    bool AudioEventContext::AddAudioFromAssetID(int assetID)
    {
        auto it = ReflectFields->AudioDataTable.find(assetID);
        if (it == ReflectFields->AudioDataTable.end() && 0 != assetID)
        {
            ReflectFields->AudioDataTable[assetID] = 1.0f;
            _totalWeight += 1.0f;
            return true;
        }
        return false;
    }

    bool AudioEventContext::RemoveAudioFromAssetID(int assetID)
    {
        auto it = ReflectFields->AudioDataTable.find(assetID);
        if (it != ReflectFields->AudioDataTable.end())
        {
            _totalWeight -= it->second;
            ReflectFields->AudioDataTable.erase(it);
            return true;
        }
        return false;
    }

    void AudioEventContext::ListenAudioFileDragDropEvent()
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();
                if (extension == L".wav")
                {
                    _newAssetID = UmFileSystem.GetAssetIDFromPath(path);
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
}
