#include "StaticMeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"

void StaticMeshRenderer::Reset() {}

void StaticMeshRenderer::Awake()
{
    //UmResourceManager.RegisterLoadQueue({L"../TestAssets/Zelda/zelda.fbx", RESOURCE_TYPE::MODEL});
}

void StaticMeshRenderer::Start()
{
    //_model = UmResourceManager.LoadResource<Model>(L"../TestAssets/Zelda/zelda.fbx");
}

void StaticMeshRenderer::OnEnable()
{
    UmRenderer.RegisterRenderQueue(this);
}

void StaticMeshRenderer::OnDisable() {}

void StaticMeshRenderer::Update() {}

void StaticMeshRenderer::FixedUpdate() {}

void StaticMeshRenderer::OnDestroy() {}

void StaticMeshRenderer::OnApplicationQuit() {}

void StaticMeshRenderer::SerializedReflectEvent() {}

void StaticMeshRenderer::DeserializedReflectEvent() {}
