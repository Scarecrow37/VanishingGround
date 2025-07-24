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
                    if (const auto extension = path.extension(); extension == L".png" || extension == L"jpeg")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this]() {
                            LoadTexture();
                            OnPlacementChange();
                        });
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
        _renderer = std::make_unique<SpriteRenderer>(_worldMatrix, SpriteType::MODE_2D);
        UmGraphics.RegisterComponent("Game", _renderer.get());
        if (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _renderer.get());
        }
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
        _guidRef = path.ToGuid();
        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this]() {
            LoadTexture();
            OnPlacementChange();
        });
    }
}

void ImageElement::OnPlacementChange()
{
    EditablePlacementUIComponent::OnPlacementChange();
    if (nullptr != _renderer)
        _renderer->SetSize(ReflectFields->Basefields.get().Basefields.get().Size);
    UpdateWorldMatrix();
}

float ImageElement::GetZOrder() const
{
    return EditablePlacementUIComponent::GetZOrder() * VIEW_ORDER_IMAGE_RATIO;
}

void ImageElement::OnSetViewOrder()
{
    EditablePlacementUIComponent::OnSetViewOrder();
    UpdateWorldMatrix();
}

void ImageElement::LoadTexture() const
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

void ImageElement::UpdateWorldMatrix()
{
    const auto [pointX, pointY] = ReflectFields->Basefields.get().Basefields.get().Point;
    const auto [scopeX, scopeY] = ReflectFields->Basefields.get().Basefields.get().ScopePoint;
    const POINT absolutePoint{.x = pointX + scopeX, .y = pointY + scopeY};
    const auto [width, height] = ReflectFields->Basefields.get().Basefields.get().Size;
    float         zOrder       = GetZOrder();
    const Vector3 position{static_cast<float>(absolutePoint.x), static_cast<float>(absolutePoint.y), zOrder};
    const Vector3 scale{static_cast<float>(width), static_cast<float>(height), 1.0f};

    _worldMatrix = /*Matrix::CreateScale(scale) * */ Matrix::CreateTranslation(position);
}