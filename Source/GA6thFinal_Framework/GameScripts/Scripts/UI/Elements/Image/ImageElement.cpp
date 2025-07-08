#include "pchScripts.h"
#include "ImageElement.h"

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
                    if (const auto extension = path.extension(); extension == L".png")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        // TODO: UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

void ImageElement::DeserializedReflectEvent()
{
    const File::Guid guid = ReflectFields->Guid;
    _guidRef              = guid;
    if (false == guid.IsNull())
    {
        // TODO: UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef);
    }
}