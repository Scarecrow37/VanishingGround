#include "pchScripts.h"
#include "ImageElement.h"
#include "Engine/GraphicsCore/SpriteRenderer.h"

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
                        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef,
                                                                              [this]() { LoadTexture(); });
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

ImageElement::~ImageElement()
{
    if (_renderer)
        _renderer->SetDestroy();
}

void ImageElement::Reset()
{
    UIComponent::Reset();
    try
    {
        _renderer = std::make_unique<SpriteRenderer>(transform->GetWorldMatrix(), SpriteType::MODE_2D);
        _renderer->RegisterRenderQueue();
        _renderer->SetActive(&EnableInHierarchy);
    }
    catch (...)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"SpriteRenderer 생성에 실패했습니다.");
        throw;
    }
}

void ImageElement::DeserializedReflectEvent()
{
    UIComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _guidRef            = path.ToGuid();
        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this]() { LoadTexture(); });
    }
}

void ImageElement::LoadTexture()
{
    if (_renderer)
    {
        const std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            const std::wstring filePath = U8ToWString(path);
            _renderer->LoadTexture(filePath);
            _size = _renderer->GetSize();
        }
    }
}