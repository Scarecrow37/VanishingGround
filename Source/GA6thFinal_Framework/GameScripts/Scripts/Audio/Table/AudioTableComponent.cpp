#include "pchScripts.h"
#include "AudioTableComponent.h"

AudioTableComponent::AudioTableComponent() = default;

struct Row
{
    void operator()(const std::string& key, const std::string& path, const bool isSelected,
                    const std::function<void()>& deleteCallback) const
    {
        // key
        ImGui::TableSetColumnIndex(0);
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        ImGui::Selectable(key.c_str(), isSelected, 0, availSize);
        ImGui::TextUnformatted(key.c_str());
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
        ImGui::InputText("##FilePath", newPathString, ImGuiInputTextFlags_ReadOnly);
        if (!newPathString->empty())
            ImGuiHelper::HoveredToolTip(*newPathString);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".wav")
                {
                    const File::Path fullPath = data->GetPath();
                    *newPathString            = fullPath.string();
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

void AudioTableComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    auto& audioMappingKeys = ReflectFields->AudioMappingKeys;

    if (ImGui::TreeNodeEx("Current Audio##details"))
    {
        if (const char* comboLabel = _selectedAudioKey.empty() ? "Select Audio" : _selectedAudioKey.c_str();
            ImGui::BeginCombo("##AudioCombo", comboLabel))
        {
            for (const auto keys = _audioSources | std::views::keys; const auto& key : keys)
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
            for (auto& [key, pathString] : audioMappingKeys)
            {
                const bool isSelected = _selectedAudioKey == key;
                ImGui::PushID(key.data());
                ImGui::TableNextRow();
                Row()(key, pathString, isSelected, [key, &eraseLater]() {
                    eraseLater(key);
                });
                ImGui::PopID();
            }

            // New Key;
            ImGui::TableNextRow();
            Row()(&_newKey, &_newPathString, audioMappingKeys.contains(_newKey), [this, &audioMappingKeys]() {
                if (auto [iterator, isSucceed] = audioMappingKeys.try_emplace(_newKey, _newPathString); isSucceed)
                {
                    LoadAudio(_newKey, File::Path(_newPathString));
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
         const auto& [key, pathString] : audioMappingKeys)
    {
        File::Path path(pathString);
        LoadAudio(key, path);
    }
}

void AudioTableComponent::LoadAudio(const std::string& key, const File::Path& path)
{
    if (path != File::NULL_PATH)
    {
        Audio::Source source = UmAudio.CreateSoundFromWave(path);
        if (const auto [iterator, isSucceed] = _audioSources.try_emplace(key, std::move(source)); false == isSucceed)
        {
            const std::string errorMsg = std::format("Audio Source with key '{}' already exists.", key);
            UmLogger.Log(LogLevel::LEVEL_WARNING, errorMsg);
        }
    }
}

void AudioTableComponent::PlaySelectedAudio()
{
    Play(_selectedAudioKey);
}

void AudioTableComponent::StopSelectedAudio()
{
    Stop(_selectedAudioKey);
}

void AudioTableComponent::Play(const std::string& key)
{
    // Check if an audio is selected
    if (true == key.empty())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "No audio selected.");
        return;
    }

    if (_audioHandles.contains(key))
    {
        if (const Audio::Handle& audioHandle = _audioHandles.at(key); UmAudio.IsValidHandle(audioHandle))
        {
            const std::string errorMessage = std::format("{} : Already playing.", key);
            UmLogger.Log(LogLevel::LEVEL_WARNING, errorMessage);
            return;
        }
    }

    // Play the selected audio
    try
    {
        const Audio::Source& audioSource = _audioSources.at(key);
        Audio::Handle        audioHandle = UmAudio.Play(audioSource);
        _audioHandles.insert_or_assign(key, std::move(audioHandle));
    }
    catch (const std::out_of_range& exception)
    {
        const std::string errorMessage = std::format("{} : Audio source does not exist.", key);
        UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
        UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
    }
}

void AudioTableComponent::Stop(const std::string& key)
{
    if (key.empty())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "No audio selected.");
        return;
    }

    try
    {
        if (const Audio::Handle& audioHandle = _audioHandles.at(key); UmAudio.IsValidHandle(audioHandle))
        {
            UmAudio.Stop(audioHandle);
            _audioHandles.insert_or_assign(key, Audio::Handle());
        }
        else
        {
            const std::string errorMessage = std::format("{} : Not playing or invalid Audio Handle.", key);
            UmLogger.Log(LogLevel::LEVEL_WARNING, errorMessage);
        }
    }
    catch (const std::out_of_range& exception)
    {
        const std::string errorMessage = std::format("{} : Audio handle does not exist.", key);
        UmLogger.Log(LogLevel::LEVEL_ERROR, errorMessage);
        UmLogger.Log(LogLevel::LEVEL_ERROR, exception.what());
    }
}
