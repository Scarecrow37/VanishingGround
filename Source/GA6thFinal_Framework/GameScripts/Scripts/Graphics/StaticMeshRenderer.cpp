#include "StaticMeshRenderer.h"
StaticMeshRenderer::StaticMeshRenderer() 
{    
    FilePath.SetDragDropFunc([this]()
        { 
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = (DragDropAsset::Data*)payLoad->Data; 
                auto context = data->pContext->lock();
                
                if (nullptr != context)
                {
                    const auto& path = context->GetPath();
                    if (path.extension() == L".fbx")
                    {
                        File::Guid guid = path.ToGuid();
                        _guidRef = UmFileSystem.GetGuidRef(guid);
                        ReflectFields->Guid = guid.string();
                        UmSceneManager.ResourceManager.RequestModelResource(this, guid);
                    }
                }
            }
    });    
}

StaticMeshRenderer::~StaticMeshRenderer()
{
    
}

void StaticMeshRenderer::Reset()
{
    MakeMeshRenderer(MESH_RENDER_TYPE::STATIC, gameObject->transform->GetWorldMatrix());
}

void StaticMeshRenderer::Awake()
{

}

void StaticMeshRenderer::Start()
{

}

void StaticMeshRenderer::OnEnable()
{
    
}

void StaticMeshRenderer::OnDisable()
{
    
}

void StaticMeshRenderer::Update() 
{
  
}

void StaticMeshRenderer::FixedUpdate() 
{

}

void StaticMeshRenderer::OnDestroy()
{
   
}

void StaticMeshRenderer::OnApplicationQuit() 
{

}

void StaticMeshRenderer::SerializedReflectEvent() 
{

}

void StaticMeshRenderer::DeserializedReflectEvent() 
{
    File::Guid guid = ReflectFields->Guid;
    _guidRef = UmFileSystem.GetGuidRef(guid);
    if (false == guid.IsNull())
    {
        UmSceneManager.ResourceManager.RequestModelResource(this, guid);
    }
}
