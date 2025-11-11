#include "pchScripts.h"
#include "StaticMeshRenderer.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"

UMREAL_COMPONENT(StaticMeshRenderer)

StaticMeshRenderer::StaticMeshRenderer() 
{    
    FilePath.SetInputAutoEvent([this]()
    { 
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path path = data->GetPath();
                const auto extension = path.extension();
                if (extension == L".fbx" || extension == L".UmModel")
                {
                    _Guid = data->GetGuid();
                    ReflectFields->Basefields.get().Guid = _Guid.string();
                    UmSceneManager.ResourceManager.RequestModelResource(this, _Guid, [this]() { LoadModel(); });
                }
            }
            ImGui::EndDragDropTarget();
        }
    });    
}

StaticMeshRenderer::~StaticMeshRenderer() = default;

void StaticMeshRenderer::LoadModel() 
{
    if (Renderer)
    {
        std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            std::wstring modelPath = U8ToWString(path);
            UmGraphics.LoadResource(modelPath, Renderer.Get());

            Renderer->OnCustomDepth(PostProcess::BLOOM);
            transform->SetChangeFlag();
            MeshComponent::InitMaterial();

            if (ReflectFields->Basefields.get().Guid == "3913ac2f-0209-4935-8835-ce051552232e")
            {
                Renderer->SetCullingEnabled(false);
            }
        }
    }
}

void StaticMeshRenderer::Reset()
{
    MakeMeshRenderer(transform->GetWorldMatrix());

    if (false == _Guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _Guid, [this]() { LoadModel(); });
    }
}

void StaticMeshRenderer::OnDrawDebugSelected()
{
    /*if (Renderer)
    {
        auto model = Renderer->GetModel();

        if (model)
        {
            const auto& meshes = model->GetMeshes();
            for (const auto& mesh : meshes)
            {
                const auto&         obb   = mesh->GetBoundingBox();
                const Matrix&       world = transform->GetWorldMatrix();
                BoundingOrientedBox worldOBB;
                obb.Transform(worldOBB, world);
                UmGraphics.DebugDraw3D("Game", worldOBB, DirectX::Colors::GreenYellow);
            }
        }
    }*/
}

void StaticMeshRenderer::OnDrawDebug()
{
    /*if (Renderer)
    {
        auto model = Renderer->GetModel();

        if (model)
        {
            const auto& meshes = model->GetMeshes();
            for (const auto& mesh : meshes)
            {
                const auto&         obb   = mesh->GetBoundingBox();
                const Matrix&       world = transform->GetWorldMatrix();
                BoundingOrientedBox worldOBB;
                obb.Transform(worldOBB, world);
                UmGraphics.DebugDraw3D("Game", worldOBB, DirectX::Colors::GreenYellow);
            }
        }
    }*/
}

void StaticMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Basefields.get().Guid;
    _Guid = guid;
}