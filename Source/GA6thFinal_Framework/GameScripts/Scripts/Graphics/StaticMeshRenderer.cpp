#include "StaticMeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"

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
    ReflectFields->Type = MeshRenderer::RENDER_TYPE::STATIC;
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
        _model = UmResourceManager.LoadResource<Model>(guid.ToPath());
    }
}

void StaticMeshRenderer::OnEnable()
{
    //if constexpr (!IS_EDITOR)
        UmRenderer.RegisterRenderQueue(this);
}

void StaticMeshRenderer::OnDisable() {}

void StaticMeshRenderer::Update() 
{
    if constexpr (IS_EDITOR)
    {
        ImGui::Begin("sdasadsadawsddasd");
        if (ImGui::Button("Load"))
        {
            File::Guid guid = ReflectFields->Guid;
            _model = UmResourceManager.LoadResource<Model>(guid.ToPath());
        }
        ImGui::End();
    }
}

void StaticMeshRenderer::FixedUpdate() {}

void StaticMeshRenderer::OnDestroy() {}

void StaticMeshRenderer::OnApplicationQuit() {}

void StaticMeshRenderer::SerializedReflectEvent() {}

void StaticMeshRenderer::DeserializedReflectEvent() {}
