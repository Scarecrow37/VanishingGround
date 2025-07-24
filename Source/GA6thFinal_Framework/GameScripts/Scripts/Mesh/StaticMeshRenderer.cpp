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
                DragDropAsset::Data* data    = (DragDropAsset::Data*)payLoad->Data;
                auto context = data->pContext->lock();
                if (nullptr != context)
                {
                    const auto& path      = context->GetPath();
                    const auto  extension = path.extension();
                    if (extension == L".fbx" || extension == L".UmModel")
                    {
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel(); });
                    }
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
        }
    }
}

void StaticMeshRenderer::Reset()
{
    __super::Reset();
    MakeMeshRenderer(MeshType::STATIC_MESH, transform->Position, transform->Scale, transform->Rotation, transform->GetWorldMatrix());
}

void StaticMeshRenderer::SerializedReflectEvent() 
{
    __super::SerializedReflectEvent();
}

void StaticMeshRenderer::DeserializedReflectEvent() 
{
    __super::DeserializedReflectEvent();

    File::Guid guid = ReflectFields->Guid;
    _guidRef = guid;
    if (false == guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, _guidRef, [this]() { LoadModel(); });
    }
}
