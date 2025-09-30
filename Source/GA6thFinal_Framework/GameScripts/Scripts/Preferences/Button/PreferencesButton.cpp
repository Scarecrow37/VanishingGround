#include "pchScripts.h"
#include "PreferencesButton.h"
#include "Scripts/UI/Elements/Image/ImageElement.h"

UMREAL_COMPONENT(PreferencesButton)

PreferencesButton::PreferencesButton()
{
    OnFilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _onImage              = data->GetGuid();
                    ReflectFields->OnGuid = _onImage.string();
                    auto image            = GetComponent<ImageElement>();
                    image->SetImage(_onImage);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    OffFilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _offImage              = data->GetGuid();
                    ReflectFields->OffGuid = _offImage.string();
                    auto image             = GetComponent<ImageElement>();
                    image->SetImage(_offImage);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void PreferencesButton::Awake()
{
    if (!_offImage.IsNull())
    {
        auto image = GetComponent<ImageElement>();
        if (image)
            image->SetImage(_offImage);
    }
}

void PreferencesButton::Start() {}

void PreferencesButton::Reset() {}

void PreferencesButton::Update() {}

void PreferencesButton::OnFocus(bool focus)
{
    auto image = GetComponent<ImageElement>();
    if (image)
    {
        if (focus)
        {
            image->SetImage(_onImage);
        }
        else
        {
            image->SetImage(_offImage);
        }
    }
}

void PreferencesButton::SerializedReflectEvent()
{
    ReflectFields->OnGuid  = _onImage.string();
    ReflectFields->OffGuid = _offImage.string();
}

void PreferencesButton::DeserializedReflectEvent()
{
    _onImage  = ReflectFields->OnGuid;
    _offImage = ReflectFields->OffGuid;
}
