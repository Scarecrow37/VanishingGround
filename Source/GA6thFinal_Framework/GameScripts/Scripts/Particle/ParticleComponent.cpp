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
                        if (_effect)  
                        {
                            _effect->SetRemoveFlag(true);
                        }
                        _effect = UmParticleManager.ParticleSerializer.Deserialize(path,false);
                        _effect->SetPlayFlag(false);
                        _effect->SetActiveFlag(false);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
    if (_effect)
    {
        for (auto emitter : _effect->GetEmitterList())
        {
            emitter->_particleRenderModule->Initialize();
        }
        _effect->SetPlayFlag(false);
    }
}
ParticleComponent::~ParticleComponent()
{
    if (_effect)
    {
        _effect->SetActiveFlag(false);
        UmParticleManager.DeleteEffect(_effect);
        _effect = nullptr;
    }
}

void ParticleComponent::Update()
{
    _effect->SetRotation(gameObject->transform->Rotation);
    _effect->SetPosition(gameObject->transform->Position);

    if (IS_EDITOR)
        if (ImGui::IsKeyDown(ImGuiKey_Space))
        {
            PlayEffect();
        }
    if (true == isplaying)
    {
        age += UmTime.DeltaTime();
        if (age >= _effect->GetLifetime())
        {
            isplaying = false;
            age       = 0;
        }
    }

}

void ParticleComponent::DeserializedReflectEvent()
{
    File::Guid guid = ReflectFields->Guid;
    _guidRef        = guid;
    _filepath       = guid;
    if (false == guid.IsNull())
    {
        _effect = UmParticleManager.ParticleSerializer.Deserialize(guid,false);
    }
}

void ParticleComponent::PlayEffect() 
{
    if (false == isplaying)
    {
        _effect->Play();
        isplaying = true;
    }
}