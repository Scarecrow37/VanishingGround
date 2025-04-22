#include "StaticMeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"

void StaticMeshRenderer::Reset() {}

void StaticMeshRenderer::Awake()
{
    UmResourceManager.RegisterLoadQueue({L"../../../Resource/TestAssets/Cerberus/cerberus.fbx", RESOURCE_TYPE::MODEL});
}

void StaticMeshRenderer::Start()
{
    _model = UmResourceManager.LoadResource<Model>(L"../../../Resource/TestAssets/Cerberus/cerberus.fbx");
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
