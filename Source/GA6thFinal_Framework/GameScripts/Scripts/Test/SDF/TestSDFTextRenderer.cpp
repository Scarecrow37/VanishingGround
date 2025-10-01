#include "pchScripts.h"
#include "TestSDFTextRenderer.h"
#include "GraphicsEngine/Interface/ISDFTextRenderer.h"

UMREAL_COMPONENT(TestSDFTextRenderer)

TestSDFTextRenderer::TestSDFTextRenderer()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                if (const auto extension = data->GetPath().extension(); extension == L".png")
                {
                    _guidRef            = data->GetGuid();
                    ReflectFields->Guid = _guidRef.string();
                    SetFont(_guidRef);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });    
}

TestSDFTextRenderer::~TestSDFTextRenderer()
{
    if (_renderer)
        _renderer->Release();
}

void TestSDFTextRenderer::SetFont(const File::GuidRef& guidRef)
{
    _guidRef = guidRef;
    ReflectFields->Guid = _guidRef.string();

    UmSceneManager.ResourceManager.RequestSDFFontResource(this, _guidRef, [this]()
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
    });
}

void TestSDFTextRenderer::Reset()
{
    UmGraphics.CreateSDFTextRenderer(&_renderer);

    UmGraphics.RegisterComponent("Game", _renderer);
    if (IS_EDITOR)
    {
        UmGraphics.RegisterComponent("Editor", _renderer);
    }
    _renderer->SetActive(&EnableInHierarchy);
}

void TestSDFTextRenderer::UpdateText() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetText(U8ToWString(ReflectFields->Text).c_str());
    }
}

void TestSDFTextRenderer::UpdateColor() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetColor(Vector4(&ReflectFields->Color[0]));
    }
}

void TestSDFTextRenderer::UpdateScale() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetFontSize(ReflectFields->FontScale);
    }
}

void TestSDFTextRenderer::UpdatePosition() const
{
    if (nullptr != _renderer)
    {
        _renderer->SetPosition(Vector3(ReflectFields->PosX, ReflectFields->PosY, 0.0f));
    }
}

void TestSDFTextRenderer::UpdateFontWeight() const
{
    if (_renderer != nullptr)
    {
        _renderer->SetFontWeight(ReflectFields->FontWeight);
    }
}