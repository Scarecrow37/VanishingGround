#include "pchScripts.h"
#include "WeaponModelManager.h"
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>

UMREAL_COMPONENT(WeaponModelManager)

WeaponModelManager::WeaponModelManager() = default;

WeaponModelManager::~WeaponModelManager() = default;

const File::Guid& WeaponModelManager::GetWeaponPrefabGuid(WeaponType type) const
{
    auto iter = _weaponPrefabGuidTable.find(type);
    if (iter != _weaponPrefabGuidTable.end())
    {
        return iter->second;
    }
    return File::NULL_GUID;
}

Vector3 WeaponModelManager::GetWeaponOffset(WeaponType type) const
{
    auto iter = _availableWeaponOffsetsTable.find(type);
    if (iter != _availableWeaponOffsetsTable.end())
    {
        return iter->second;
    }
    return Vector3::Zero;
}

WeaponModelData WeaponModelManager::RequestAvailableWeapon(WeaponType type)
{
    auto& animationPool    = _weaponAnimationTable[type];
    auto& particlePool     = _weaponParticleTable[type];
    auto& availableIndices = _availableWeaponIndicesTable[type];
    if (false == availableIndices.empty())
    {
        size_t index = availableIndices.top();
        availableIndices.pop();
        AnimationComponent* animation  = animationPool[index];
        ParticleComponent*  particle   = particlePool[index];
        GameObject*         gameObject = animation ? &animation->gameObject : nullptr;

        if (gameObject)
        {
            gameObject->ActiveSelf = true;
        }

        return WeaponModelData(index, type, gameObject, animation, particle);
    }
    return WeaponModelData();
}

bool WeaponModelManager::ReturnWeaponModel(WeaponModelData data)
{
    if (data.IsValid())
    {
        if (GameObject* object = &data.Animation->gameObject)
        {
            _availableWeaponIndicesTable[data.Type].push(data._index);
            object->ActiveSelf  = false;
            data.Animation      = nullptr;
            data.Particle       = nullptr;
            data.GameObject     = nullptr;
            return true;
        }
    }
    return false;
}

const std::string* WeaponModelManager::GetRandomWeaPonAnimationKeyToNormalAttack(WeaponType type)
{
    auto& animList = _weaponAnimationNormalNameList[type];
    if (false == animList.empty())
    {
        size_t randomIndex = Random::Index(animList.size());
        return &animList[randomIndex];
    }
    return nullptr;
}

const std::string* WeaponModelManager::GetRandomWeaPonAnimationKeyToSpecialAttack(WeaponType type)
{
    auto& animList = _weaponAnimationSpecialNameList[type];
    if (false == animList.empty())
    {
        size_t randomIndex = Random::Index(animList.size());
        return &animList[randomIndex];
    }
    return nullptr;
}

void WeaponModelManager::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING,
                     (const char*)u8"WeaponModelManager는 싱글톤 컴포넌트입니다. 중복 생성할 수 없습니다.");
        return;
    }
}

void WeaponModelManager::Start()
{
    for (auto& [type, guid] : _weaponPrefabGuidTable)
    {
        const auto* prefab = UmGameObjectFactory.GetOriginPrefab(guid);
        if (prefab && prefab->front())
        {
            for (size_t i = 0; i < WEAPON_POOLING_SIZE; ++i)
            {
                GameObject* clone = GameObject::Instantiate(prefab->front().get());
                if (clone)
                {
                    clone->transform->SetParent(transform, false);
                    std::string typeStr = rfl::enum_to_string(type);
                    clone->AddTag(typeStr.c_str());
                    clone->SetActive(false);
                    AnimationComponent* animation = clone->GetComponent<AnimationComponent>();
                    ParticleComponent*  particle  = clone->GetComponent<ParticleComponent>();
                    if (animation)
                    {
                        RegisterWeaponAnimation(type, animation);
                    }
                    if (particle)
                    {
                        RegisterWeaponParticle(type, particle);
                    }
                    _availableWeaponIndicesTable[type].push(i);
                }
            }
        }
    }
    InitializeAnimationList();
}

void WeaponModelManager::Update() {}

void WeaponModelManager::OnDestroy() {}

void WeaponModelManager::SerializedReflectEvent() {}

void WeaponModelManager::DeserializedReflectEvent()
{
    for (auto& [typeStr, guid] : ReflectFields->WeaponPrefabGuidTable)
    {
        auto type = rfl::string_to_enum<WeaponType>(typeStr);
        if (type)
        {
            _weaponPrefabGuidTable[type.value()] = File::Guid(guid);
        }
    }
    UpdateOffsetPosition();
}

void WeaponModelManager::ImGuiDrawPropertysEvent()
{
    constexpr auto   WeaponTypeArray     = rfl::get_enumerator_array<WeaponType>();
    constexpr size_t WeaponTypeArraySize = WeaponTypeArray.size();
    if (ImGui::TreeNodeEx("Weapon Prefab##weapon_manager", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& [name, type] : WeaponTypeArray)
        {
            ImGui::PushID(name.data());

            std::string& guidStr = ReflectFields->WeaponPrefabGuidTable[name.data()];
            File::Guid&  guid    = _weaponPrefabGuidTable[type];
            std::string  pathStr = guid.ToPath().string();

            ImGuiHelper::AlignedText(name.data(), ImGuiHelper::LEFT);
            ImGuiHelper::TextWithVerticalSeparator("Prefab", 150.0f);
            ImGui::SameLine();
            ImGui::InputText("##weapon_guid", &pathStr, ImGuiInputTextFlags_ReadOnly);
            ImGuiHelper::HoveredToolTip(pathStr.c_str());
            if (false == UmCore->IsPlay() && ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                {
                    DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                    const File::Path&    path      = data->GetPath();
                    const File::Path&    extension = path.extension();
                    if (extension == L".UmPrefab")
                    {
                        guidStr = data->GetGuid().string();
                        guid    = data->GetGuid();
                    }
                    else
                    {
                        UmLogger.Log(LogLevel::LEVEL_WARNING,
                                     (const char*)u8"프리팹은 .UmPrefab 파일만 지정할 수 있습니다.");
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGuiHelper::TextWithVerticalSeparator("Offset", 150.0f);
            if (ImGui::DragFloat3("##offset", ReflectFields->WeaponPrefabOffsetTable[name.data()].data(), 1.0f))
            {
                UpdateOffsetPosition();
            }

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Invalid Instances", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& [name, type] : WeaponTypeArray)
        {
            ImGui::PushID(name.data());
            ImGuiHelper::TextWithVerticalSeparator(name.data(), 150.0f);
            ImGui::SameLine();
            size_t availableCount = _availableWeaponIndicesTable[type].size();
            size_t totalCount     = _weaponAnimationTable[type].size();
            ImGui::Text("%s : %zu / %zu", name.data(), availableCount, totalCount);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

void WeaponModelManager::UpdateOffsetPosition()
{
    _availableWeaponOffsetsTable.clear();
    for (auto& [typeStr, offsetArray] : ReflectFields->WeaponPrefabOffsetTable)
    {
        auto type = rfl::string_to_enum<WeaponType>(typeStr);
        if (type)
        {
            Vector3 offset;
            offset = Vector3(offsetArray[0], offsetArray[1], offsetArray[2]);
            _availableWeaponOffsetsTable[type.value()] = offset;
        }
    }
}

void WeaponModelManager::RegisterWeaponAnimation(WeaponType type, AnimationComponent* component)
{
    if (component)
    {
        
        _weaponAnimationTable[type].push_back(component);
    }
}

void WeaponModelManager::RegisterWeaponParticle(WeaponType type, ParticleComponent* component)
{
    if (component)
    {
        _weaponParticleTable[type].push_back(component);
    }
}

void WeaponModelManager::InitializeAnimationList() 
{
    for (auto& [type, animPool] : _weaponAnimationTable)
    {
        auto& normalAnimList  = _weaponAnimationNormalNameList[type];
        auto& specialAnimList = _weaponAnimationSpecialNameList[type];
        if (false == animPool.empty())
        {
            if (const auto& anim = animPool.front())
            {
                const auto& keyMap = anim->GetAnimationKeyMap();
                for (auto& [key, anim] : keyMap)
                {
                    if (key.find("_S_") != std::string::npos)
                    { // "_S_" 포함된 키 → 스페셜 리스트
                        specialAnimList.push_back(key);
                    }
                    else
                    { // 나머지 → 일반 리스트
                        normalAnimList.push_back(key);
                    }
                }
            }
        }
    }
}

