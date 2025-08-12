#include "pchScripts.h"
#include <Mesh/SkeletalMeshRenderer.h>
#include "ParticleComponent.h"


ParticleComponent::ParticleComponent()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path path = data->GetPath();
                File::Guid guid = data->GetGuid();
                const auto extension = path.extension();
                if (extension == L".vfx")
                {
                    _filepath = path;
                    _guidRef  = guid;
                    ReflectFields->Guid = _guidRef.string();
                    LoadParticle();
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

    if (ImGui::IsKeyPressed(ImGuiKey_K))
    {
        PlayEffect();
    }


}

void ParticleComponent::Start() 
{
    _skelMesh = GetComponent<SkeletalMeshRenderer>();
}

void ParticleComponent::Reset() 
{

}

void ParticleComponent::SerializedReflectEvent()
{
    ReflectFields->PositionArray[0] = Position->x;
    ReflectFields->PositionArray[1] = Position->y;
    ReflectFields->PositionArray[2] = Position->z;

    ReflectFields->RotationArray[0] = Rotation->x;
    ReflectFields->RotationArray[1] = Rotation->y;
    ReflectFields->RotationArray[2] = Rotation->z;

    ReflectFields->ScaleArray[0] = Scale->x;
    ReflectFields->ScaleArray[1] = Scale->y;
    ReflectFields->ScaleArray[2] = Scale->z;

    //ReflectFields->AttachToBoneMatrix = _effect->_followBoneFlag;

    ReflectFields->Guid = _guidRef.string();


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
    SkeletalMeshRenderer* skelMesh = GetComponent<SkeletalMeshRenderer>();
    if (nullptr != skelMesh)
    {

        auto&       renderer   = skelMesh->Renderer;
        auto&       model      = renderer->GetModel();
        const auto& _boneNames = model->GetBoneNameList();

        const char* comboLabel = (nullptr != skelMesh) ? ReflectFields->BoneNameToAttach.c_str() : "-";
        if (ImGui::BeginCombo("##bone name", comboLabel))
        {
            for (int i = 0; i < _boneNames.size(); ++i)
            {
                bool isSelected = ReflectFields->BoneNameToAttach == _boneNames[i];
                if (ImGui::Selectable(_boneNames[i].c_str(), isSelected))
                {
                    ReflectFields->BoneNameToAttach = _boneNames[i];
                }
            }
            ImGui::EndCombo();
        }
    }


    if (ImGui::Button("Play"))
    {
        if (IS_EDITOR)
        {
            PlayEffect();
        }
        
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        if (IS_EDITOR)
        {
            StopEffect();
        }
    }
}



void ParticleComponent::LoadParticle() 
{
    if (_effect)
    {
        _effect->SetRemoveFlag(true);
    }
    if (_filepath == L"")
        return;
    UmParticleSerializer.PreDeserialize(_filepath);
    const auto& modelpaths = UmParticleSerializer.GetUsedModelPaths();
    
    for (const auto& path : modelpaths)
    {
        File::Path texPath = path;
        texPath            = std::filesystem::absolute(texPath).generic_string();
        File::Guid guid    = texPath.ToGuid();
        UmSceneManager.ResourceManager.RequestModelResource(this, guid, []() {});
    }

    const auto& paths = UmParticleSerializer.GetUsedTexturePaths();
    size_t count = 0;
    for (const auto& path : paths)
    {
        File::Path texPath = path;
        texPath            = std::filesystem::absolute(texPath).generic_string();
        File::Guid guid    = texPath.ToGuid();
        count++;
        if (count == paths.size())
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
                //


            });
        else
            UmSceneManager.ResourceManager.RequestTextureResource(this, guid, []() {});
    }
}


void ParticleComponent::FollowBoneMatrix() 
{
    if (true == AttachToBoneMatrix)
    {

        SkeletalMeshRenderer* skelMesh = GetComponent<SkeletalMeshRenderer>();

        if (skelMesh != nullptr)
        {
            if (ReflectFields->BoneNameToAttach != "")
            {

                _effect->_boneWorldMatrix =
                    skelMesh->Renderer->GetAnimator()->FindBoneMatrix(ReflectFields->BoneNameToAttach.c_str());
                _effect->_followBoneFlag = true;
            }
        }
    }
    else
    {
        _effect->_followBoneFlag = false;

    }


}

void ParticleComponent::StopEffect()
{
    if (nullptr != _effect)
    {
        _effect->Stop();
    }
}

void ParticleComponent::PlayEffect() 
{
    if (nullptr != _effect)
    {

        FollowBoneMatrix();
        _effect->Play();
    }


}