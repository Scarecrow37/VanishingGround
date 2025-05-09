#include "StaticMeshRenderer.h"
#include "Engine/GraphicsCore/MeshRenderer.h"

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
    if (_meshRenderer)
        _meshRenderer->SetDestroy();
}

void StaticMeshRenderer::Reset()
{
    _meshRenderer = std::make_unique<MeshRenderer>(MeshRenderer::RENDER_TYPE::STATIC, transform->GetWorldMatrix());
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
    _meshRenderer->SetActive(true);
}

void StaticMeshRenderer::OnDisable()
{
    _meshRenderer->SetActive(false);
}

void StaticMeshRenderer::Update() 
{
    if constexpr (IS_EDITOR)
    {
        ImGui::Begin("sdasadsadawsddasd");
        if (ImGui::Button("Load"))
        {
            File::Guid guid = ReflectFields->Guid;
            _meshRenderer->LoadModel(guid.ToPath().c_str());
        }
        ImGui::End();
    }
}

void StaticMeshRenderer::FixedUpdate() 
{

}

void StaticMeshRenderer::OnDestroy()
{
   
}

void StaticMeshRenderer::OnApplicationQuit() {}

void StaticMeshRenderer::SerializedReflectEvent() {}

void StaticMeshRenderer::DeserializedReflectEvent() {}
