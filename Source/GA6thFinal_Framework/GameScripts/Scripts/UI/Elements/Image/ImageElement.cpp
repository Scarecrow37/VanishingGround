#include "pchScripts.h"
#include "ImageElement.h"
#include "GraphicsEngine/Interface/ISpriteRenderer.h"

UMREAL_COMPONENT(ImageElement)

ImageElement::ImageElement() : _renderer(nullptr), _opacityFactor(1.0f)
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {

                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png" || extension == L".jpeg")
                {
                    _Guid               = data->GetGuid();
                    ReflectFields->Guid = _Guid.string();
                    RequestResource();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

ImageElement::~ImageElement() = default;

void ImageElement::SetImage(const File::Guid& Guid)
{
    _Guid = Guid;
    ReflectFields->Guid = _Guid.string();
    RequestResource();
}

void ImageElement::SetLinearFill(const float fill) const
{
    _renderer->SetLinearFill(fill);
}

void ImageElement::SetRadialFill(const float fill) const
{
    _renderer->SetRadialFill(fill);
}

void ImageElement::Reset()
{
    DrawUIComponent::Reset();

    try
    {
        UmGraphics.CreateSpriteRenderer(&_renderer, &_worldMatrix);
        UmGraphics.RegisterComponent("Game", _renderer.Get());

        if (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _renderer.Get());
        }

        _renderer->SetType(UIType::MODE_2D);
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
        _Guid = path.ToGuid();
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

void ImageElement::UpdateAtlas()
{
    if (_renderer)
    {
        _renderer->SetAtlas(ReflectFields->Column, ReflectFields->Row);
    }
}

void ImageElement::UpdateAtlasIndex()
{
    if (_renderer)
    {
        _renderer->SetAtlasIndex(ReflectFields->ColumnIndex, ReflectFields->RowIndex);
    }
}

void ImageElement::ResetToSpriteSize()
{
    const int column = ReflectFields->Column;
    const int row    = ReflectFields->Row;
    _requestedSize = SIZE{_spriteOriginSize.cx / column, _spriteOriginSize.cy / row};
    InvalidateMeasure();
}

void ImageElement::SetOpacity(const float opacity)
{
    const float clampedAlpha = std::clamp(opacity, 0.0f, 1.0f);
    ReflectFields->Alpha     = clampedAlpha;
    UpdateRendererAlpha();
}

void ImageElement::SetOpacityFactor(const float factor)
{
    _opacityFactor = factor;
    UpdateRendererAlpha();
}

void ImageElement::BindResourceLoadedCallback(ResourceLoadedCallback callback)
{
    _resourceLoadedCallback = std::move(callback);
}

void ImageElement::LoadTexture(const File::Guid& guid) const
{
    if (nullptr != _renderer)
    {
        const std::string path = guid.ToPath().string();
        if (path != File::NULL_PATH)
        {
            const std::wstring filePath = U8ToWString(path);
            UmGraphics.LoadResource(filePath, _renderer.Get());
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

void ImageElement::UpdateRendererAlpha() const
{
    if (nullptr != _renderer)
    {
        const float alpha = ReflectFields->Alpha * _opacityFactor;
        _renderer->SetAlpha(alpha);
    }
}

void ImageElement::RequestResource() 
{
    if (false == _Guid.IsNull())
    {
        File::Guid requestedGuid = _Guid;
        UmSceneManager.ResourceManager.RequestTextureResource(this, _Guid, [this, requestedGuid]() {
            LoadTexture(requestedGuid);
            UpdateWorldMatrix();
            if (_renderer)
            {
                _spriteOriginSize = _renderer->GetSize();
            }

            const SIZE size = Size;
            UpdateRendererSize(size);

            UpdateRendererAlpha();

            UpdateAtlas();
            UpdateAtlasIndex();

            const float linearFill = LinearFill;
            SetLinearFill(linearFill);

            const float radialFill = RadialFill;
            SetRadialFill(radialFill);

            // ResetToSpriteSize();
            if (_resourceLoadedCallback)
            {
                CallbackParameters params;
                params.ResourceSize = _spriteOriginSize;
                _resourceLoadedCallback(params);
                _resourceLoadedCallback = nullptr;
            }
        });
    }
}