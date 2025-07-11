#include "pchScripts.h"
#include "TestText.h"
#include "Engine/GraphicsCore/FontRenderer.h"

TestText::TestText()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data    = (DragDropAsset::Data*)payLoad->Data;
                auto                 context = data->pContext->lock();
                if (nullptr != context)
                {
                    const auto& path      = context->GetPath();
                    const auto  extension = path.extension();
                    if (extension == L".sfont")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();

                        UmSceneManager.ResourceManager.RequestFontResource(this, _guidRef, [this]() { LoadFont(); });
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

TestText::~TestText()
{
    if (_fontRenderer)
        _fontRenderer->SetDestroy();
}

void TestText::Reset()
{
    _fontRenderer = std::make_unique<FontRenderer>();
    _fontRenderer->RegisterComponent();
    _fontRenderer->SetActive(&EnableInHierarchy);
}

void TestText::ImGuiDrawPropertysEvent()
{
    Vector4 color = _fontRenderer->GetColor();
    if (ImGui::ColorEdit4("Color##text", &color.x))
    {
        _fontRenderer->SetColor(color);
    }

    Vector3 position = _fontRenderer->GetPosition();
    if (ImGui::DragFloat3("Position##text", &position.x))
    {
        _fontRenderer->SetPosition(position);
    }

    Vector2 scale = _fontRenderer->GetScale();
    if (ImGui::DragFloat2("Scale##text", &scale.x))
    {
        _fontRenderer->SetScale(scale);
    }

    Vector2 origin = _fontRenderer->GetOrigin();
    if (ImGui::DragFloat2("Origin##text", &origin.x))
    {
        _fontRenderer->SetOrigin(origin);
    }

    std::string text;
    if (ImGui::InputText("Text##text", &text))
    {
        std::wstring t = U8ToWString(text);
        _fontRenderer->SetText(t);
    }
}

void TestText::DeserializedReflectEvent()
{
    File::Guid guid = ReflectFields->Guid;
    auto       path = guid.ToPath();

    if (!path.IsNull())
    {
        _guidRef            = path.ToGuid();
        ReflectFields->Guid = _guidRef.string();

        UmSceneManager.ResourceManager.RequestFontResource(this, _guidRef, [this]() { LoadFont(); });
    }
}

void TestText::LoadFont()
{
    if (_fontRenderer)
    {
        std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            std::wstring filePath = U8ToWString(path);
            _fontRenderer->LoadFont(filePath);
        }
    }
}