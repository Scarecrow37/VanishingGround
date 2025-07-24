#include "pchScripts.h"
#include "TextElement.h"

TextElement::TextElement()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data    = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto context = data->pContext->lock(); nullptr != context)
                {
                    const auto& path      = context->GetPath();
                    if (const auto extension = path.extension(); extension == L".UmFont")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        RequestResource();
                    }
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

void TextElement::Reset()
{
    EditablePlacementUIComponent::Reset();

    try
    {
        _renderer = std::make_unique<FontRenderer>();
        UmGraphics.RegisterComponent("Game", _renderer.get());
        if (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _renderer.get());
        }
        _renderer->SetActive(&EnableInHierarchy);
    }
    catch (...)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, u8"FontRenderer 생성에 실패했습니다.");
        throw;
    }
}

void TextElement::DeserializedReflectEvent()
{
    EditablePlacementUIComponent::DeserializedReflectEvent();

    const File::Guid guid = ReflectFields->Guid;
    if (const auto path = guid.ToPath(); !path.IsNull())
    {
        _guidRef = path.ToGuid();
        RequestResource();
    }
}

void TextElement::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();

    UpdatePosition();
    if (ReflectFields->IsFitContent)
    {
        FitContent();
    }
}

float TextElement::GetZOrder() const
{
    return EditablePlacementUIComponent::GetZOrder() * VIEW_ORDER_TEXT_RATIO + VIEW_ORDER_TEXT_OFFSET;
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
    EditablePlacementUIComponent::SetViewOrder(viewOrder);

    UpdatePosition();
}

void TextElement::PassProperty() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetOrigin(Vector2::Zero);
        _renderer->SetRotation(0.0f);
    }
    UpdateAll();
}

void TextElement::UpdateAll() const
{
    UpdateText();
    UpdateColor();
    UpdatePosition();
    UpdateScale();
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
        const auto& [x, y]   = GetAbsolutePoint();
        const float   zOrder = GetZOrder();
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

void TextElement::FitContent()
{
    if (nullptr != _renderer)
    {
        XMFLOAT2 size;
        XMStoreFloat2(&size, _renderer->GetStringSize());
        const SIZE newSize{.cx = static_cast<LONG>(size.x), .cy = static_cast<LONG>(size.y)};
        ReflectFields->Basefields.get().Basefields.get().Size = newSize;
    }
}

void TextElement::RequestResource() const
{
    UmSceneManager.ResourceManager.RequestFontResource(this, _guidRef, [this]() {
        LoadFont();
        PassProperty();
    });
}