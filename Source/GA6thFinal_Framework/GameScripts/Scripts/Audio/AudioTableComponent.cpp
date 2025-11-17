#include "pchScripts.h"
#include "AudioTableComponent.h"

REGISTER_CLASS(Timeline::EventTrack, Timeline::AudioEventContext)
UMREAL_COMPONENT(AudioTableComponent)

AudioTableComponent::AudioTableComponent() = default;

namespace
{
    struct Row
    {
        void operator()(const std::string& key, const std::string& path, const bool isSelected,
                        const std::function<void()>& deleteCallback) const
        {
            // key
            ImGui::TableSetColumnIndex(0);
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::Selectable(key.c_str(), isSelected, 0, ImVec2(availSize.x, 0.0f));
            ImGuiHelper::HoveredToolTip(key);

            // path
            ImGui::TableSetColumnIndex(1);
            const File::Path fullPath(path);
            const File::Path fileName       = fullPath.filename();
            std::string      fileNameString = fileName.string();
            availSize                       = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.0f);
            ImGui::InputText("##FilePath", &fileNameString, ImGuiInputTextFlags_ReadOnly);
            ImGuiHelper::HoveredToolTip(fileNameString);

            // Delete button
            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(height, height)))
            {
                deleteCallback();
            }
        }

        void operator()(std::string* newKey, std::string* newPathString, const bool isExisted,
                        const std::function<void()>& addCallback) const
        {
            if (newKey == nullptr || newPathString == nullptr)
            {
                return;
            }
            // key
            ImGui::TableSetColumnIndex(0);
            ImVec2 availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x);
            ImGuiHelper::StyleBuilder style;
            if (newKey->empty() || isExisted)
                style.PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.15f, 0.25f, 1.0f));
            else
                style.PushStyleColor(ImGuiCol_Text, ImVec4(0.15f, 0.75f, 0.35f, 1.0f));
            ImGui::InputTextWithHint("##NewKey", "New Key...", newKey);
            style.PopStyle();

            // path
            ImGui::TableSetColumnIndex(1);
            availSize = ImGui::GetContentRegionAvail();
            ImGui::SetNextItemWidth(availSize.x - 60.0f);
            const int flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_EnterReturnsTrue;
            if (ImGui::InputText("##FilePath", newPathString, flags))
            {
                addCallback();
            }
            if (!newPathString->empty())
                ImGuiHelper::HoveredToolTip(*newPathString);
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                {
                    const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                    if (const auto extension = data->GetPath().extension(); extension == L".wav")
                    {
                        const File::Path& fullPath  = data->GetPath();
                        const int         assetId   = UmFileSystem.GetAssetIDFromPath(fullPath);
                        if (0 != assetId)
                        {
                            *newKey = std::to_string(assetId);
                        }
                        *newPathString = fullPath.string();
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // Delete button
            const float height = ImGui::GetItemRectSize().y;
            ImGui::SameLine();
            if (!newKey->empty() && !newPathString->empty() && !isExisted && ImGui::Button("+", ImVec2(height, height)))
            {
                addCallback();
            }
        }
    };

    struct EraseLater
    {
        using Map = std::unordered_map<std::string, std::string>;
        explicit EraseLater(Map* map) : MapPointer(map) {}
        EraseLater(const EraseLater&)                = delete;
        EraseLater& operator=(const EraseLater&)     = delete;
        EraseLater(EraseLater&&) noexcept            = delete;
        EraseLater& operator=(EraseLater&&) noexcept = delete;
        ~EraseLater()
        {
            for (const auto& key : KeysToErase)
            {
                MapPointer->erase(key);
            }
            KeysToErase.clear();
        }

        void operator()(const std::string& key) { KeysToErase.push_back(key); }

        Map*                     MapPointer;
        std::vector<std::string> KeysToErase;
    };

    struct ImGuiDisabled
    {
        explicit ImGuiDisabled(const bool condition = true) : Condition(condition)
        {
            if (Condition)
                ImGui::BeginDisabled();
        }
        ImGuiDisabled(const ImGuiDisabled&)                = delete;
        ImGuiDisabled& operator=(const ImGuiDisabled&)     = delete;
        ImGuiDisabled(ImGuiDisabled&&) noexcept            = delete;
        ImGuiDisabled& operator=(ImGuiDisabled&&) noexcept = delete;
        ~ImGuiDisabled()
        {
            if (Condition)
                ImGui::EndDisabled();
        }

        bool Condition;
    };
}


void AudioTableComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    auto& audioMappingKeys = ReflectFields->AudioMappingKeys;

    if (ImGui::TreeNodeEx("Current Audio##details"))
    {
        if (const char* comboLabel = _selectedAudioKey.empty() ? "Select Audio" : _selectedAudioKey.c_str();
            ImGui::BeginCombo("##AudioCombo", comboLabel))
        {
            for (const auto keys = audioMappingKeys | std::views::keys; const auto& key : keys)
            {
                if (const bool isSelected = _selectedAudioKey == key; ImGui::Selectable(key.c_str(), isSelected))
                {
                    _selectedAudioKey = key;
                }
            }
            ImGui::EndCombo();
        }

        {
            [[maybe_unused]] ImGuiDisabled disabled(_selectedAudioKey.empty());
            // Play
            {
                if (ImGui::Button(EditorIcon::ICON_PLAY))
                    PlaySelectedAudio();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Play");
            }
            ImGui::SameLine();
            // Stop
            {
                if (ImGui::Button(EditorIcon::ICON_STOP))
                    StopSelectedAudio();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Stop");
            }
            ImGui::SameLine();
            // FadeIn
            {
                if (ImGui::Button(EditorIcon::ICON_BELL_ON))
                    UmAudio.ReverbOn();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Enable Reverb Effect");
            }
            ImGui::SameLine();
            // FadeOut
            {
                if (ImGui::Button(EditorIcon::ICON_BELL_OFF))
                    UmAudio.ReverbOff();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Clear Reverb Effects");
            }
            ImGui::SameLine();
            // FadeIn
            {
                if (ImGui::Button(EditorIcon::ICON_CIRCLE_ARROW_RIGHT))
                    UmAudio.FadeIn();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Fade In");
            }
            ImGui::SameLine();
            // FadeOut
            {
                if (ImGui::Button(EditorIcon::ICON_CIRCLE_ARROW_LEFT))
                    UmAudio.FadeOut();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Fade Out");
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Audio Mapping Keys##details"))
    {
        // AddMapping Function
        if (ImGui::BeginTable("NotifyTable##Details", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            // Headers
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Audio File", ImGuiTableColumnFlags_WidthStretch, 0.7f);
            ImGui::TableHeadersRow();

            // Existing Keys
            EraseLater eraseLater(&audioMappingKeys);
            for (auto& [key, guidString] : audioMappingKeys)
            {
                const bool isSelected = _selectedAudioKey == key;
                ImGui::PushID(key.data());
                ImGui::TableNextRow();
                File::Guid  guid(guidString); 
                File::Path  path           = guid.ToPath();
                File::Path  filename       = path.filename();
                std::string filenameString = filename.string();
                Row()(key, filenameString, isSelected, [key, &eraseLater]() {
                    eraseLater(key);
                });
                ImGui::PopID();
            }

            // New Key;
            ImGui::TableNextRow();
            Row()(&_newKey, &_newPathString, audioMappingKeys.contains(_newKey), [this, &audioMappingKeys]() {
                const File::Guid newPathGuid = File::Path(_newPathString).ToGuid();
                std::string guidString  = newPathGuid.string();
                if (auto [iterator, isSucceed] = audioMappingKeys.try_emplace(_newKey, guidString); isSucceed)
                {
                    LoadAudio(_newKey, newPathGuid);
                    _newKey.clear();
                    _newPathString.clear();
                }
            });
            ImGui::EndTable();
        }

        ImGui::TreePop();
    }
}

void AudioTableComponent::Reset()
{
    Component::Reset();

    LoadAudio();
}

void AudioTableComponent::LoadAudio()
{
    for (const auto& audioMappingKeys = ReflectFields->AudioMappingKeys;
         const auto& [key, guidString] : audioMappingKeys)
    {
        File::Guid guid(guidString);
        LoadAudio(key, guid);
    }
}

void AudioTableComponent::LoadAudio(const std::string& key, const File::Guid& guid)
{
    UmAudio.LoadSound(key, guid);
}

void AudioTableComponent::PlaySelectedAudio()
{
    auto [iter, succeed] = _audioHandles.try_emplace(_selectedAudioKey, std::vector<Audio::AudioHandle>());
    std::vector<Audio::AudioHandle>& handles = iter->second;
    const Audio::AudioHandle handle = UmAudio.Play(_selectedAudioKey);
    handles.push_back(handle);
}

void AudioTableComponent::StopSelectedAudio()
{
    try
    {
        std::vector<Audio::AudioHandle>& handles = _audioHandles.at(_selectedAudioKey);
        std::ranges::for_each(handles, [](const Audio::AudioHandle& handle) {
            UmAudio.Stop(handle); });
        handles.clear();
    }
    catch (const std::out_of_range& exception)
    {
        const std::string errorMessage = std::format("{} : Audio source does not played.", _selectedAudioKey);
        UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
        UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
    }
}