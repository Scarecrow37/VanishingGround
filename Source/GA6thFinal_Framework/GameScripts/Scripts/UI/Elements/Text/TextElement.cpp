#include "pchScripts.h"
#include "TextElement.h"
#include "GraphicsEngine/Interface/ISDFTextRenderer.h"

UMREAL_COMPONENT(TextElement)

TextElement::TextElement()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png")
                {
                    _Guid            = data->GetGuid();
                    ReflectFields->Guid = _Guid.string();
                    RequestResource();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

TextElement::~TextElement()
{
    if (_renderer)
        _renderer->Release();
}

void TextElement::SetFont(const File::Guid& guid)
{
    _Guid = guid;
    ReflectFields->Guid = _Guid.string();
    RequestResource();
}

void TextElement::SetOpacity(const float opacity)
{
    const float clampedOpacity = std::clamp(opacity, 0.0f, 1.0f);
    ReflectFields->Color[3]    = clampedOpacity;
    UpdateColor();
    ReflectFields->FontOutlineColor[3] = clampedOpacity;
    UpdateOutline();
}

void TextElement::Reset()
{
    DrawUIComponent::Reset();

    try
    {
        UmGraphics.CreateSDFTextRenderer(&_renderer);
        UmGraphics.RegisterComponent("Game", _renderer);
        if (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _renderer);
        }
        _renderer->SetActive(&EnableInHierarchy);

        RequestResource();
    }
    catch (...)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"FontRenderer 생성에 실패했습니다.");
        throw;
    }
}

void TextElement::DeserializedReflectEvent()
{
    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _Guid = path.ToGuid();
    }
}

float TextElement::GetZOrder() const
{
    return DrawUIComponent::GetZOrder() * VIEW_ORDER_TEXT_RATIO + VIEW_ORDER_TEXT_OFFSET;
}

void TextElement::ImGuiDrawPropertysEvent()
{
    DrawUIComponent::ImGuiDrawPropertysEvent();

    if (_isDebug)
    {
        const std::string& guid = ReflectFields->Guid;
        ImGuiDebug()("GUID", guid);

        const auto [cx, cy] = ReflectFields->ContentSize;
        ImGuiDebug()("Content Size", cx, cy);
    }
}

SIZE TextElement::MeasureOverride(const SIZE availableSize)
{
    const FillMode horizontalFillMode = HorizontalFillMode;
    const FillMode verticalFillMode   = VerticalFillMode;

    SIZE desiredSize = MinSize()(availableSize, _requestedSize, horizontalFillMode == FillMode::FILL,
                                 verticalFillMode == FillMode::FILL);

    const auto [contentWidth, contentHeight] = ReflectFields->ContentSize;

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = contentWidth;
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = contentHeight;

    return desiredSize;
}

SIZE TextElement::ArrangeOverride(const SIZE finalSize)
{
    const SIZE desiredSize = DesiredSize;
    const SIZE actualSize  = MinSize()(finalSize, desiredSize);

    UpdatePosition();

    return actualSize;
}

void TextElement::LoadFont() const
{
    if (nullptr != _renderer)
    {
        const std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            const std::wstring filePath = U8ToWString(path);
            UmGraphics.LoadResource(filePath, _renderer);
        }
    }
}

void TextElement::SetViewOrder(const int viewOrder)
{
    DrawUIComponent::SetViewOrder(viewOrder);

    UpdatePosition();
}

void TextElement::UpdateProperties()
{
    UpdateText();
    UpdateColor();
    UpdatePosition();
    UpdateScale();
    UpdateOutline();
    UpdateContentSize();
}

void TextElement::UpdateText() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetText(U8ToWString(ReflectFields->Text).c_str());
    }
}

void TextElement::UpdateColor() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetColor(Vector4(&ReflectFields->Color[0]));
    }
}

void TextElement::UpdatePosition() const
{
    if (nullptr != _renderer)
    {
        const POINT absolutePosition = AbsolutePosition;
        const auto& [x, y]           = absolutePosition;
        const float   zOrder         = GetZOrder();
        const Vector3 position{static_cast<float>(x), static_cast<float>(y), zOrder};
        _renderer->SetPosition(position);
    }
}

void TextElement::UpdateScale() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetFontSize(ReflectFields->FontScale);
    }
}

void TextElement::UpdateWeight() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetFontWeight(ReflectFields->FontWeight);
    }
}

void TextElement::UpdateContentSize()
{
    if (nullptr != _renderer)
    {
        XMFLOAT2 size;
        XMStoreFloat2(&size, _renderer->GetStringSize());
        ReflectFields->ContentSize = SIZE{.cx = static_cast<LONG>(size.x), .cy = static_cast<LONG>(size.y)};
    }
    else
    {
        ReflectFields->ContentSize = SIZE{0, 0};
    }
}

void TextElement::UpdateOutline()
{
    if (nullptr != _renderer)
    {
        const UINT fontFlags = ReflectFields->FontFlags;
        const bool enabled   = fontFlags & FONT_FLAG_OUTLINE;

        const SimpleMath::Color outlineColor = SimpleMath::Color(&ReflectFields->FontOutlineColor[0]);
        const float             outlineWidth = ReflectFields->FontOutlineWidth;

        const GE::FontOutline outline{.Color = outlineColor, .Width = outlineWidth, .Enabled = enabled};

        _renderer->SetFontOutline(outline);
    }
}

void TextElement::RequestResource()
{
    if (false == _Guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestSDFFontResource(this, _Guid, [this]() {
            LoadFont();
            UpdateProperties();
        });
    }
}