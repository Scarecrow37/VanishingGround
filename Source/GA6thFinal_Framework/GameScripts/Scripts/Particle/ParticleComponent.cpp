#include "pchScripts.h"
#include "Mesh/SkeletalMeshRenderer.h"
#include "ParticleComponent.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"
#include "GraphicsEngine/Interface/IAnimator.h"

UMREAL_COMPONENT(ParticleComponent)

ParticleComponent::ParticleComponent()
{
    FilePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                const File::Path&    path      = data->GetPath();
                const File::Guid&    guid      = data->GetGuid();
                const auto           extension = path.extension();
                if (extension == L".vfx" && _newEffectKey != "")
                {
                    ReflectFields->EffectNameTable.push_back(_newEffectKey);
                    ReflectFields->GuidMap[_newEffectKey]       = guid.string();
                    ReflectFields->AttachFlagMap[_newEffectKey] = false;
                    ReflectFields->BoneNameMap[_newEffectKey]   = "";
                    LoadParticle(_newEffectKey);
                    _newEffectKey = "";
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
}

ParticleComponent::~ParticleComponent()
{    
    for (auto& key : ReflectFields->EffectNameTable)
    {
        UmParticleManager->SetActiveFlag(this, key, false);
        UmParticleManager->DeleteEffect(this, key, "Game");
    }
}

void ParticleComponent::SerializedReflectEvent()
{
    for (auto& keyString : ReflectFields->EffectNameTable)
    {
        ReflectFields->TranslationMap[keyString][0] = _positionVector[keyString].x;
        ReflectFields->TranslationMap[keyString][1] = _positionVector[keyString].y;
        ReflectFields->TranslationMap[keyString][2] = _positionVector[keyString].z;

        ReflectFields->RotationMap[keyString][0] = _rotationVector[keyString].x;
        ReflectFields->RotationMap[keyString][1] = _rotationVector[keyString].y;
        ReflectFields->RotationMap[keyString][2] = _rotationVector[keyString].z;

        ReflectFields->ScaleMap[keyString][0] = _scaleVector[keyString].x;
        ReflectFields->ScaleMap[keyString][1] = _scaleVector[keyString].y;
        ReflectFields->ScaleMap[keyString][2] = _scaleVector[keyString].z;
    }
}

void ParticleComponent::DeserializedReflectEvent()
{
    for (auto& key : ReflectFields->EffectNameTable)
    {
        LoadParticle(key);
        _positionVector[key] = Vector3(ReflectFields->TranslationMap[key][0], ReflectFields->TranslationMap[key][1],
                                       ReflectFields->TranslationMap[key][2]);
        _rotationVector[key] = Vector3(ReflectFields->RotationMap[key][0], ReflectFields->RotationMap[key][1],
                                       ReflectFields->RotationMap[key][2]);
        _scaleVector[key] =
            Vector3(ReflectFields->ScaleMap[key][0], ReflectFields->ScaleMap[key][1], ReflectFields->ScaleMap[key][2]);
    }
}

void ParticleComponent::ImGuiDrawPropertysEvent()
{
    if (false == ReflectFields->EffectNameTable.empty())
    {
        const char* comboLabel = _currentEffectKey.c_str();
        if (ImGui::BeginCombo("##current effect name", comboLabel))
        {
            for (int i = 0; i < ReflectFields->EffectNameTable.size(); ++i)
            {
                bool isSelected = _currentEffectKey == ReflectFields->EffectNameTable[i];
                if (ImGui::Selectable(ReflectFields->EffectNameTable[i].c_str(), isSelected))
                {
                    _currentEffectKey = ReflectFields->EffectNameTable[i];
                }
            }
            ImGui::EndCombo();
        }
        auto it = ReflectFields->GuidMap.find(_currentEffectKey);
        if (it != ReflectFields->GuidMap.end())
        {
            const File::Guid&  fileGuid   = (*it).second;
            const File::Path&  filePath   = fileGuid.ToPath();
            std::string fileString = filePath.string();
            ImGui::Text(fileString.c_str());
        }
    }
    SkeletalMeshRenderer* skelMesh = GetComponent<SkeletalMeshRenderer>();
    if (nullptr != skelMesh)
    {
        auto& renderer = skelMesh->Renderer;
        if (nullptr != renderer)
        {
            const auto& model = renderer->GetModel();
            if (nullptr != model)
            {
                const auto& _boneNames = model->GetBoneNameList();
                if (false == _boneNames.empty())
                {
                    auto it = ReflectFields->BoneNameMap.find(_currentEffectKey);
                    if (it != ReflectFields->BoneNameMap.end())
                    {
                        const char* comboLabel =
                            (nullptr != skelMesh) ? ReflectFields->BoneNameMap[_currentEffectKey].c_str() : "-";
                        if (ImGui::BeginCombo("##bone name", comboLabel))
                        {
                            for (int i = 0; i < _boneNames.size(); ++i)
                            {
                                bool isSelected = ReflectFields->BoneNameMap[_currentEffectKey] == _boneNames[i];
                                if (ImGui::Selectable(_boneNames[i].c_str(), isSelected))
                                {
                                    ReflectFields->BoneNameMap[_currentEffectKey] = _boneNames[i];
                                    FollowBoneMatrix(_currentEffectKey);
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                }
            }
        }
    }

    if (ImGui::Button("Play Current Effect"))
    {
        if (IS_EDITOR)
        {
            PlayEffect(_currentEffectKey);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop Current Effect"))
    {
        if (IS_EDITOR)
        {
            StopEffect(_currentEffectKey);
        }
    }

    if (ImGui::Button("Play All Effect"))
    {
        if (IS_EDITOR)
        {
            for (auto& key : ReflectFields->EffectNameTable)
                PlayEffect(key);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop All Effect"))
    {
        if (IS_EDITOR)
        {
            for (auto& key : ReflectFields->EffectNameTable)
                StopEffect(key);
        }
    }

    if (ImGui::Button("Clear All Effect"))
    {
        if (IS_EDITOR)
        {
            ClearEffectList();
        }
    }
}

void ParticleComponent::Start()
{
    FollowBoneMatrix();
}

//void ParticleComponent::Update()
//{
//    if (ImGui::IsKeyPressed(ImGuiKey_K))
//    {
//        PlayEffect("focus");
//    }
//    if (ImGui::IsKeyPressed(ImGuiKey_L))
//    {
//        StopEffect("focus");
//    }
//    if (ImGui::IsKeyPressed(ImGuiKey_N))
//    {
//        StopEffect("buff");
//    }
//    if (ImGui::IsKeyPressed(ImGuiKey_M))
//    {
//        StopEffect("debuff");
//    }
//}

void ParticleComponent::LoadParticle(const std::string& keyString)
{
    auto it = ReflectFields->GuidMap.find(keyString);
    if (it != ReflectFields->GuidMap.end())
    {
        File::Guid assetGuid = (*it).second;
        if (assetGuid == File::NULL_GUID)
            return;
        UmParticleSerializer.PreDeserialize(assetGuid.ToPath());
        const auto& modelpaths = UmParticleSerializer.GetUsedModelPaths();

        if (modelpaths.empty())
        {
            const auto& paths = UmParticleSerializer.GetUsedTexturePaths();
            size_t      count = 0;
            for (const auto& path : paths)
            {
                File::Path texPath = path;
                texPath            = std::filesystem::absolute(texPath).generic_string();
                File::Guid guid    = texPath.ToGuid();
                count++;

                if (count == paths.size())
                    UmSceneManager.ResourceManager.RequestTextureResource(this, guid, [this, assetGuid, keyString]() {
                        ReflectFields->GuidMap[keyString] = assetGuid.string();
                        auto effect =
                            UmParticleSerializer.Deserialize(this, keyString, assetGuid.ToPath(), false, "Game");
                        if (effect)
                        {
                            for (auto& emitter : effect->GetEmitterList())
                            {
                                File::Path absolutePath = emitter->_particleRenderModule->GetModelAndTexturePath();
                                absolutePath            = std::filesystem::absolute(absolutePath).generic_string();
                                UmGraphics.LoadTextureResource(std::wstring_view(absolutePath.wstring()), emitter.get());
                            }
                            effect->SetPlayFlag(false);
                            effect->SetActiveFlag(false);
                            effect->SetPosition(&_positionVector[keyString]);
                            effect->SetRotation(&_rotationVector[keyString]);
                            effect->SetScale(&_scaleVector[keyString]);
                            effect->SetParentMatrix(&transform->GetWorldMatrix());
                            effect->SetBoneFollowFlag(&(ReflectFields->AttachFlagMap[keyString]));
                        }
                    });
                else
                    UmSceneManager.ResourceManager.RequestTextureResource(this, guid, []() {});
            }
        }
        else
        {
            const auto& paths = UmParticleSerializer.GetUsedTexturePaths();
            size_t      count = 0;
            for (const auto& path : paths)
            {
                File::Path texPath = path;
                texPath            = std::filesystem::absolute(texPath).generic_string();
                File::Guid guid    = texPath.ToGuid();
                count++;
                if (count != paths.size())
                    UmSceneManager.ResourceManager.RequestTextureResource(this, guid, []() {});
                else
                {
                    UmSceneManager.ResourceManager.RequestTextureResource(
                        this, guid, [this, modelpaths, assetGuid, keyString]() {
                            size_t count = 0;
                            for (const auto& path : modelpaths)
                            {
                                File::Path texPath = path;
                                texPath            = std::filesystem::absolute(texPath).generic_string();
                                File::Guid guid    = texPath.ToGuid();
                                count++;
                                if (count != modelpaths.size())
                                    UmSceneManager.ResourceManager.RequestModelResource(this, guid, []() {});
                                else
                                    UmSceneManager.ResourceManager.RequestModelResource(
                                        this, guid, [this, assetGuid, keyString]() {
                                            ReflectFields->GuidMap[keyString] = assetGuid.string();
                                            auto effect                       = UmParticleSerializer.Deserialize(
                                                this, keyString, assetGuid.ToPath(), false, "Game");
                                            if (effect)
                                            {
                                                for (auto& emitter : effect->GetEmitterList())
                                                {
                                                    File::Path absolutePath =
                                                        emitter->_particleRenderModule->GetModelAndTexturePath();
                                                    absolutePath =
                                                        std::filesystem::absolute(absolutePath).generic_string();
                                                    UmGraphics.LoadTextureResource(
                                                        std::wstring_view(absolutePath.wstring()), emitter.get());

                                                    if (LocationShape::MESH_SURFACE == emitter->_locationType)
                                                    {
                                                        if (auto locator =
                                                                emitter->_emitLocator->AsMeshSurfaceLocator())
                                                        {
                                                            File::Path absolutePath = locator->GetModelPath();
                                                            absolutePath = std::filesystem::absolute(absolutePath)
                                                                               .generic_string();
                                                            UmGraphics.LoadModelResource(
                                                                std::wstring_view(absolutePath.wstring()), emitter.get());
                                                        }
                                                    }
                                                }
                                                effect->SetPlayFlag(false);
                                                effect->SetActiveFlag(false);
                                                effect->SetPosition(&_positionVector[keyString]);
                                                effect->SetRotation(&_rotationVector[keyString]);
                                                effect->SetScale(&_scaleVector[keyString]);
                                                effect->SetParentMatrix(&transform->GetWorldMatrix());
                                                effect->SetBoneFollowFlag(&(ReflectFields->AttachFlagMap[keyString]));
                                            }
                                        });
                            }
                        });
                }
            }
        }
    }
}

void ParticleComponent::FollowBoneMatrix(const std::string& key)
{
    auto it = ReflectFields->AttachFlagMap.find(key);
    if (it != ReflectFields->AttachFlagMap.end())
    {
        if (true == (*it).second)
        {
            SkeletalMeshRenderer* skelMesh = GetComponent<SkeletalMeshRenderer>();
            if (skelMesh != nullptr)
            {
                auto it2 = ReflectFields->BoneNameMap.find(key);
                if (it2 != ReflectFields->BoneNameMap.end())
                {
                    if (skelMesh->Renderer && skelMesh->Renderer->GetAnimator())
                    {
                        const Matrix* boneMat =
                            skelMesh->Renderer->GetAnimator()->FindBoneMatrix(ReflectFields->BoneNameMap[key].c_str());

                        UmParticleManager->SetBoneMatrix(this, key, boneMat);
                    }
                }
            }
        }
    }
}

void ParticleComponent::FollowBoneMatrix()
{
    for (auto& keyString : ReflectFields->EffectNameTable)
    {
        FollowBoneMatrix(keyString);
    }
}

void ParticleComponent::SetAnimator(class Animator* animator)
{
    _animator = animator;
    for (auto& keyString : ReflectFields->EffectNameTable)
    {
        FollowBoneMatrix(keyString);
    }
}

void ParticleComponent::PlayEffect(const std::string& key)
{
    FollowBoneMatrix(key);
    UmParticleManager->PlayEffect(this, key);
}

void ParticleComponent::PlayEffect(const std::string& key, EffectCallback callback) 
{
    FollowBoneMatrix(key);
    UmParticleManager->PlayEffect(this, key,callback);
}

void ParticleComponent::StopEffect(const std::string& key)
{
    UmParticleManager->StopEffect(this, key);
}

void ParticleComponent::StopAll()
{
    for (auto& key : ReflectFields->EffectNameTable)
    {
        UmParticleManager->StopEffect(this, key);
    }
}

void ParticleComponent::ClearEffectList()
{
    for (auto& effectKey : ReflectFields->EffectNameTable)
    {
        UmParticleManager->DeleteEffect(this, effectKey, "Game");
    }

    ReflectFields->EffectNameTable.clear();
    ReflectFields->GuidMap.clear();
    ReflectFields->BoneNameMap.clear();
    ReflectFields->AttachFlagMap.clear();
    ReflectFields->ScaleMap.clear();
    ReflectFields->RotationMap.clear();
    ReflectFields->TranslationMap.clear();
    _positionVector.clear();
    _rotationVector.clear();
    _scaleVector.clear();
}

void ParticleComponent::RegisterEffectFromGuid(const File::Path& filepath, const std::string& key)
{
    File::Guid guid = filepath.ToGuid();
    if (false == guid.IsNull())
    {
        ReflectFields->GuidMap[key] = guid.string();
        ReflectFields->EffectNameTable.push_back(key);
        LoadParticle(key);
    }
}

void ParticleComponent::RegisterEffectFromGuid(const File::Guid& fileguid, const std::string& key)
{
    if (false == fileguid.IsNull())
    {
        ReflectFields->GuidMap[key] = fileguid.string();
        ReflectFields->EffectNameTable.push_back(key);
        LoadParticle(key);
    }
}
