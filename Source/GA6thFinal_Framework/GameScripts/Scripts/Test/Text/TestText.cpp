#include "pchScripts.h"
#include "TestText.h"

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

    ReflectFields->Color = {1.0f, 1.0f, 1.0f, 1.0f};
    ReflectFields->Scale = {1.f, 1.f};
}

TestText::~TestText()
{
    if (_fontRenderer)
        _fontRenderer->SetDestroy();
}

void TestText::Reset()
{
    _fontRenderer = std::make_unique<FontRenderer>();
    _fontRenderer->SetActive(&EnableInHierarchy);
    UmGraphics.RegisterComponent("Game", _fontRenderer.get());
    if constexpr (IS_EDITOR)
    {
        UmGraphics.RegisterComponent("Editor", _fontRenderer.get());
    }
}

void TestText::ImGuiDrawPropertysEvent()
{
    if (ImGui::ColorEdit4("Color##text", &ReflectFields->Color[0]))
    {
        _fontRenderer->SetColor(Vector4(&ReflectFields->Color[0]));
    }

    if (ImGui::DragFloat3("Position##text", &ReflectFields->Position[0]))
    {
        _fontRenderer->SetPosition(Vector3(&ReflectFields->Position[0]));
    }

    if (ImGui::DragFloat2("Scale##text", &ReflectFields->Scale[0]))
    {
        _fontRenderer->SetScale(Vector2(&ReflectFields->Scale[0]));
    }

    if (ImGui::DragFloat2("Origin##text", &ReflectFields->Origin[0]))
    {
        _fontRenderer->SetOrigin(Vector2(&ReflectFields->Origin[0]));
    }

    if (ImGui::InputText("Text##text", &ReflectFields->Text))
    {
        std::wstring t = U8ToWString(ReflectFields->Text);
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
            UmGraphics.LoadResource(filePath, _fontRenderer.get());

            _fontRenderer->SetText(U8ToWString(ReflectFields->Text));
            _fontRenderer->SetColor(Vector4(&ReflectFields->Color[0]));
            _fontRenderer->SetPosition(Vector3(&ReflectFields->Position[0]));
            _fontRenderer->SetScale(Vector2(&ReflectFields->Scale[0]));
            _fontRenderer->SetOrigin(Vector2(&ReflectFields->Origin[0]));
            _fontRenderer->SetRotation(ReflectFields->Rotation);
        }
    }
}