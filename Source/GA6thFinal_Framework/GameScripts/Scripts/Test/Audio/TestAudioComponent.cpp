#include "pchScripts.h"
#include "TestAudioComponent.h"

UMREAL_COMPONENT(TestAudioComponent)

TestAudioComponent::TestAudioComponent()
{
    Path.SetInputAutoEvent([this] {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = (DragDropAsset::Data*)payLoad->Data;
                _filePath = data->GetPath();
                UmAudio.LoadSound(_filePath.string(), data->GetGuid());
            }

            ImGui::EndDragDropTarget();
        }
    });
}
TestAudioComponent::~TestAudioComponent() = default;

void TestAudioComponent::ImGuiDrawPropertysEvent() 
{
    ImGui::SliderFloat("Master Volume", &_masterVolume, 0.0f, 1.0f, "%.1f");

    bool isBGM = _group == Audio::GROUP_BGM;
    if (ImGui::RadioButton("GROUP_BGM", &isBGM))
    {
        _group = Audio::GROUP_BGM;
    }
    ImGui::SameLine();
    bool isEffect = _group == Audio::GROUP_EFFECT;
    if (ImGui::RadioButton("GROUP_EFFECT", &isEffect))
    {
        _group = Audio::GROUP_EFFECT;
    }

    if (false == _filePath.IsNull())
    {
        {
            if (ImGui::Button(EditorIcon::ICON_PLAY))
            {
                _hAudio = UmAudio.Play(_filePath.string(), _group);
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Play");
        }
        ImGui::SameLine();
        // Stop
        {
            if (ImGui::Button(EditorIcon::ICON_STOP))
            {
                UmAudio.Stop(_hAudio);
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Stop");
        }
    }
}
