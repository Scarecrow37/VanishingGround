#include "StaticMeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"

void StaticMeshRenderer::Reset()
{
    ReflectFields->Type = MeshRenderer::RENDER_TYPE::STATIC;
}

void StaticMeshRenderer::Awake()
{
    UmResourceManager.RegisterLoadQueue({L"../../../Resource/TestAssets/Zelda/zelda.fbx", RESOURCE_TYPE::MODEL});
}

void StaticMeshRenderer::Start()
{
    _model = UmResourceManager.LoadResource<Model>(L"../../../Resource/TestAssets/Zelda/zelda.fbx");
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
