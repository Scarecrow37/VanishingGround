#include "pchScripts.h"
#include "ParticleComponent.h"


ParticleComponent::ParticleComponent()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data    = (DragDropAsset::Data*)payLoad->Data;
                auto                 context = data->pContext->lock();
                if (nullptr != context)
                {
                    const auto& path      = context->GetPath();
                    const auto  extension = path.extension();
                    if (extension == L".vfx")
                    {
                        _filepath           = path;
                        _guidRef            = path.ToGuid();
                        ReflectFields->Guid = _guidRef.string();
                        LoadParticle();
                    
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
    if (_effect)
    {
        _effect->SetPlayFlag(false);
    }
}
ParticleComponent::~ParticleComponent()
{
    if (_effect)
    {
        _effect->SetActiveFlag(false);
        UmParticleManager->DeleteEffect(_effect);
        _effect = nullptr;
    }
}

void ParticleComponent::Update()
{
    




}

void ParticleComponent::DeserializedReflectEvent()
{
    File::Guid guid = ReflectFields->Guid;
    _guidRef        = guid;
    _filepath       = guid.ToPath();
    if (false == guid.IsNull())
    {
        LoadParticle();
    }
    Position = Vector3(ReflectFields->PositionArray[0], ReflectFields->PositionArray[1], ReflectFields->PositionArray[2]);
    Rotation = Vector3(ReflectFields->RotationArray[0], ReflectFields->RotationArray[1], ReflectFields->RotationArray[2]);
    Scale = Vector3(ReflectFields->ScaleArray[0], ReflectFields->ScaleArray[1], ReflectFields->ScaleArray[2]);

}

void ParticleComponent::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Play"))
    {
        if (IS_EDITOR)
        {
            PlayEffect();
        }
        
    }
}

void ParticleComponent::LoadParticle() 
{
    if (_effect)
    {
        _effect->SetRemoveFlag(true);
    }
    UmParticleSerializer.PreDeserialize(_filepath);
    const auto& modelpaths = UmParticleSerializer.GetUsedModelPaths();
    
    for (int i = 0; i < modelpaths.size(); ++i)
    {
        File::Path texPath = modelpaths[i];
        texPath            = std::filesystem::absolute(texPath).generic_string();
        File::Guid guid    = texPath.ToGuid();
        UmSceneManager.ResourceManager.RequestModelResource(this, guid, []() {});
    }
    const auto& paths = UmParticleSerializer.GetUsedTexturePaths();
    for (int i = 0; i < paths.size(); ++i)
    {
        File::Path texPath = paths[i];
        texPath            = std::filesystem::absolute(texPath).generic_string();
        File::Guid guid    = texPath.ToGuid();
        if (i == paths.size() - 1)
            UmSceneManager.ResourceManager.RequestTextureResource(this, guid, [this]() 
                {
                _effect = UmParticleSerializer.Deserialize(_filepath, false, "Game");
                for (auto& emitter : _effect->GetEmitterList())
                {
                    File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                    absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                    UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter);

                    if (LocationShape::MESH_SURFACE == emitter->_locationType)
                    {
                        MeshSurfaceLocator* locator      = static_cast<MeshSurfaceLocator*>(emitter->_emitLocator);
                        File::Path absolutePath = locator->GetModelPath();
                        absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                        UmGraphics.LoadModelResource(std::wstring_view(absolutePath.wstring()), emitter);
                    }

                }
                _effect->SetPlayFlag(false);
                _effect->SetActiveFlag(false);
                _effect->_position = &_positionVector;
                _effect->_rotation = &_rotationVector;
                _effect->_scale    = &_scaleVector;
                _effect->_parentWorldMatrix = &transform->GetWorldMatrix();
            });
        else
            UmSceneManager.ResourceManager.RequestTextureResource(this, guid, []() {});
    }
}

void ParticleComponent::PlayEffect() 
{
    _effect->Play();

}