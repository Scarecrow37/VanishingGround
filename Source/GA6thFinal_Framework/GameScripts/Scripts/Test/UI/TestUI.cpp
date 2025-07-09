#include "pchScripts.h"
#include "TestUI.h"
#include "Engine/GraphicsCore/SpriteRenderer.h"

TestUI::TestUI()
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
                    if (extension == L".png" || extension == L".jpeg")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();

                        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this]() { LoadTexture(); });
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

TestUI::~TestUI()
{
    if (_spriteRenderer)
        _spriteRenderer->SetDestroy();
}

void TestUI::Reset()
{
    _spriteRenderer = std::make_unique<SpriteRenderer>(transform->GetWorldMatrix(), SpriteType::MODE_2D);
    _spriteRenderer->RegisterRenderQueue();
    _spriteRenderer->SetActive(&EnableInHierarchy);
}

void TestUI::ImGuiDrawPropertysEvent()
{
    const char* texts[] = {"2D", "2.5D", "3D"};
    if (ImGui::Combo("UIType##ui", (int*)&ReflectFields->Type, texts, 3))
    {
        _spriteRenderer->SetType((SpriteType)ReflectFields->Type);
    }

    if (ImGui::DragInt2("Size##ui", (int*)&_size))
    {
        _spriteRenderer->SetSize(_size);
    }
}

void TestUI::DeserializedReflectEvent()
{
    File::Guid guid = ReflectFields->Guid;
    auto       path = guid.ToPath();

    if (!path.IsNull())
    {
        _guidRef            = path.ToGuid();
        ReflectFields->Guid = _guidRef.string();

        UmSceneManager.ResourceManager.RequestTextureResource(this, _guidRef, [this]() { LoadTexture(); });

        _spriteRenderer->SetType((SpriteType)ReflectFields->Type);
    }
}

void TestUI::LoadTexture()
{
    if (_spriteRenderer)
    {
        std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            std::wstring filePath = U8ToWString(path);
            _spriteRenderer->LoadTexture(filePath);
            _size = _spriteRenderer->GetSize();
        }
    }
}