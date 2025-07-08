#include "pchScripts.h"
#include "TestUI.h"
#include "Engine/GraphicsCore/UIRenderer.h"

TestUI::TestUI() : _type(UIType::MODE_2D)
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

                        _uiRenderer->LoadTexture(path.c_str());
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

TestUI::~TestUI()
{
    if (_uiRenderer)
        _uiRenderer->SetDestroy();
}

void TestUI::Reset()
{
    _uiRenderer = std::make_unique<UIRenderer>(transform->GetWorldMatrix(), UIType::MODE_2D);
    _uiRenderer->RegisterRenderQueue();
    _uiRenderer->SetActive(&_isActive);
}

void TestUI::ImGuiDrawPropertysEvent()
{
    const char* texts[] = {"2D", "2.5D", "3D"};
    if (ImGui::Combo("UIType##ui", (int*)&_type, texts, 3))
    {
        _uiRenderer->SetType(_type);
    }
}
