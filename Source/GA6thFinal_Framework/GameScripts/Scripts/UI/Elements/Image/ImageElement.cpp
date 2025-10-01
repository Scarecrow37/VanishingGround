#include "pchScripts.h"
#include "ImageElement.h"

UMREAL_COMPONENT(ImageElement)

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
    {
        _renderer->SetDestroy();
        _renderer = nullptr;
    }
}

void ImageElement::SetImage(const File::Guid& guidRef)
{
    _guidRef = guidRef;
    ReflectFields->Guid = _guidRef.string();
    RequestResource();
}

void ImageElement::SetLinearFill(float fill)
{
    _renderer->SetLinearFill(fill);
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

float ImageElement::GetZOrder() const
{
    return DrawUIComponent::GetZOrder() * VIEW_ORDER_IMAGE_RATIO;
}

void ImageElement::SetViewOrder(const int viewOrder)
{
    DrawUIComponent::SetViewOrder(viewOrder);

    UpdateWorldMatrix();
}

void ImageElement::ImGuiDrawPropertysEvent()
{
    DrawUIComponent::ImGuiDrawPropertysEvent();

    if (ImGui::Button("Reset to Sprite Size"))
    {
        ResetToSpriteSize();
    }

    if (_isDebug)
    {
        ImGuiDebug()("Sprite Size", _spriteOriginSize.cx, _spriteOriginSize.cy);
        const std::string& guid = ReflectFields->Guid;
        ImGuiDebug()("GUID", guid);
    }
}

SIZE ImageElement::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;
    const SIZE     desiredSize        = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                       verticalFillMode == FillMode::FILL);

    return desiredSize;
}

SIZE ImageElement::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize  = MinSize()(finalSize, desiredSize);

    UpdateRendererSize(actualSize);
    UpdateWorldMatrix();

    return actualSize;
}

void ImageElement::ResetToSpriteSize()
{
    _requestedSize = _spriteOriginSize;
    InvalidateMeasure();
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
    const POINT absolutePosition = AbsolutePosition;
    const auto& [x, y]           = absolutePosition;
    const float zOrder           = GetZOrder();

    const Vector3 position{static_cast<float>(x), static_cast<float>(y), zOrder};

    _worldMatrix = Matrix::CreateTranslation(position);
}

void ImageElement::UpdateRendererSize(const SIZE size) const
{
    if (nullptr != _renderer)
    {
        _renderer->SetSize(size);
    }
}

void ImageElement::UpdateRendererAlpha(const float alpha) const
{
    if (nullptr != _renderer)
    {
        _renderer->SetAlpha(alpha);
    }
}

void ImageElement::RequestResource() 
{
    if (false == _guidRef.IsNull())
    {
        File::GuidRef requestedGuid = _guidRef;
        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this, requestedGuid]() {
            LoadTexture(requestedGuid);
            UpdateWorldMatrix();
            _spriteOriginSize = _renderer->GetSize();

            const SIZE size = Size;
            UpdateRendererSize(size);

            const float alpha = Alpha;
            UpdateRendererAlpha(alpha);

            //ResetToSpriteSize();
        });
    }
}