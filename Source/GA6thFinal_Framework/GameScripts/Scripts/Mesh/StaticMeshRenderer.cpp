#include "pchScripts.h"
#include "StaticMeshRenderer.h"

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
                    _guidRef = data->GetGuid();
                    ReflectFields->Basefields.get().Guid = _guidRef.string();
                    UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel(); });
                }
            }
            ImGui::EndDragDropTarget();
        }
    });    
}

StaticMeshRenderer::~StaticMeshRenderer() 
{

}

void StaticMeshRenderer::LoadModel() 
{
    if (Renderer)
    {
        std::string path = FilePath;
        if (path != File::NULL_PATH)
        {
            std::wstring modelPath = U8ToWString(path);
            UmGraphics.LoadResource(modelPath, Renderer.get());
            Renderer->OnCustomDepth(PostProcess::BLOOM);
            __super::InitMaterial();
        }
    }
}

void StaticMeshRenderer::Reset()
{
    MakeMeshRenderer(MeshType::STATIC_MESH, transform->Position, transform->Scale, transform->Rotation,
                     transform->GetWorldMatrix());

    if (false == _guidRef.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel(); });
    }
}

void StaticMeshRenderer::SerializedReflectEvent() 
{
}

void StaticMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Basefields.get().Guid;
    _guidRef = guid;
}