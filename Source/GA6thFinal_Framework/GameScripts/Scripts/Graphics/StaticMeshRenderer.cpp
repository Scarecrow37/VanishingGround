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
                        ReflectFields->Guid = path.ToGuid().string();
                        UmResourceManager.RegisterLoadQueue({path, RESOURCE_TYPE::MODEL});
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
    MakeMeshRenderer(MeshRenderer::RENDER_TYPE::STATIC, gameObject->transform->GetWorldMatrix());
    _meshRenderer->RegisterRenderQueue("Editor");
}

void StaticMeshRenderer::Awake()
{
    if (!ReflectFields->Guid.empty())
    {
        File::Guid guid = ReflectFields->Guid;
        UmResourceManager.RegisterLoadQueue({guid.ToPath(), RESOURCE_TYPE::MODEL});
    }
}

void StaticMeshRenderer::Start()
{
    if (!ReflectFields->Guid.empty())
    {
        File::Guid guid = ReflectFields->Guid;
        _meshRenderer->LoadModel(guid.ToPath().c_str());
    }
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

}
