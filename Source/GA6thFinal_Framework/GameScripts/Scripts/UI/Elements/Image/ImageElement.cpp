#include "pchScripts.h"
#include "ImageElement.h"
#include "Engine/GraphicsCore/UIRenderer.h"

ImageElement::ImageElement()
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
                    if (const auto extension = path.extension(); extension == L".png" || extension == L"jpeg")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        _renderer->LoadTexture(path.c_str());
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void ImageElement::Reset()
{
    UIComponent::Reset();
    _renderer = std::make_unique<UIRenderer>(transform->GetWorldMatrix(), UIType::MODE_2D);
    _renderer->RegisterRenderQueue();
    _renderer->SetActive(&EnableInHierarchy);
}

void ImageElement::OnDestroy()
{
    UIComponent::OnDestroy();
    if (_renderer)
        _renderer->SetDestroy();
}