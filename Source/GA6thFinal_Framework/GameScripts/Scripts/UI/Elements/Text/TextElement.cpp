#include "pchScripts.h"
#include "TextElement.h"

UMREAL_COMPONENT(TextElement)

TextElement::TextElement()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".UmFont")
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

TextElement::~TextElement()
{
    if (_renderer)
        _renderer->SetDestroy();
}

void TextElement::SetFont(const File::GuidRef& guidRef)
{
    _guidRef = guidRef;
    ReflectFields->Guid = _guidRef.string();
    RequestResource();
}

void TextElement::Reset()
{
    DrawUIComponent::Reset();

    try
    {
        _renderer = std::make_unique<FontRenderer>();
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
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"FontRenderer 생성에 실패했습니다.");
        throw;
    }
}

void TextElement::DeserializedReflectEvent()
{
    DrawUIComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _guidRef = path.ToGuid();
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
    const LONG scaledContentWidth  = static_cast<LONG>(static_cast<float>(contentWidth) * ReflectFields->FontScale);
    const LONG scaledContentHeight = static_cast<LONG>(static_cast<float>(contentHeight) * ReflectFields->FontScale);

    if (horizontalFillMode == FillMode::WRAP)
        desiredSize.cx = scaledContentWidth;
    if (verticalFillMode == FillMode::WRAP)
        desiredSize.cy = scaledContentHeight;

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
            UmGraphics.LoadResource(filePath, _renderer.get());
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
    if (nullptr != _renderer)
    {
        _renderer->SetOrigin(Vector2::Zero);
        _renderer->SetRotation(0.0f);
    }
    UpdateText();
    UpdateColor();
    UpdatePosition();
    UpdateScale();
    UpdateContentSize();
}

void TextElement::UpdateText() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetText(U8ToWString(ReflectFields->Text));
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
        const Vector2 scale{ReflectFields->FontScale, ReflectFields->FontScale};
        _renderer->SetScale(scale);
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

void TextElement::RequestResource()
{
    if (false == _guidRef.IsNull())
    {
        UmSceneManager.ResourceManager.RequestFontResource(this, _guidRef, [this]() {
            LoadFont();
            UpdateProperties();
        });
    }
}