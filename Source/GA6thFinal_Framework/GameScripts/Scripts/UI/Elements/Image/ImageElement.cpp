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
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _guidRef            = data->GetGuid();
                    ReflectFields->Guid = _guidRef.string();
                    RequestResource();
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

void ImageElement::SetImage(const File::GuidRef& guidRef)
{
    _guidRef = guidRef;
    ReflectFields->Guid = _guidRef.string();
    RequestResource();
}

void ImageElement::Reset()
{
    DrawUIComponent::Reset();

    try
    {
        _renderer = std::make_unique<SpriteRenderer>(_worldMatrix, SpriteType::MODE_2D);
        UmGraphics.RegisterComponent("Game", _renderer.get());
        if (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _renderer.get());
        }
        _renderer->SetActive(&EnableInHierarchy);

        RequestResource();
    }
    catch (...)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"SpriteRenderer 생성에 실패했습니다.");
        throw;
    }
}

void ImageElement::DeserializedReflectEvent()
{
    DrawUIComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _guidRef = path.ToGuid();
    }
}

void ImageElement::OnPlacementChange()
{
    DrawUIComponent::OnPlacementChange();

    if (nullptr != _renderer)
    {
        const SIZE size = GetSize();
        _renderer->SetSize(size);
    }
    UpdateWorldMatrix();
}

float ImageElement::GetZOrder() const
{
    return DrawUIComponent::GetZOrder() * VIEW_ORDER_IMAGE_RATIO;
}

void ImageElement::SetViewOrder(const int viewOrder)
{
    DrawUIComponent::SetViewOrder(viewOrder);

    UpdateWorldMatrix();
}

void ImageElement::LoadTexture() const
{
    LoadTexture(_guidRef);
}

void ImageElement::LoadTexture(const File::GuidRef& guid) const
{
    if (nullptr != _renderer)
    {
        const std::string path = guid.ToPath().string();
        if (path != File::NULL_PATH)
        {
            const std::wstring filePath = U8ToWString(path);
            UmGraphics.LoadResource(filePath, _renderer.get());
        }
    }
}

void ImageElement::UpdateWorldMatrix()
{
    const auto& [x, y] = GetAbsolutePoint();
    const float zOrder = GetZOrder();

    const Vector3 position{static_cast<float>(x), static_cast<float>(y), zOrder};

    _worldMatrix = Matrix::CreateTranslation(position);
}

void ImageElement::RequestResource()
{
    if (false == _guidRef.IsNull())
    {
        File::GuidRef requestedGuid = _guidRef;
        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this, requestedGuid]() {
            LoadTexture(requestedGuid);
            OnPlacementChange();
        });
    }
}