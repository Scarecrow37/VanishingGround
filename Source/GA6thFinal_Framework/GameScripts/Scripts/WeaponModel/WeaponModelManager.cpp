#include "pchScripts.h"
#include "WeaponModelManager.h"
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>

UMREAL_COMPONENT(WeaponModelManager)

WeaponModelManager::WeaponModelManager() = default;

WeaponModelManager::~WeaponModelManager() = default;

const File::Guid& WeaponModelManager::GetWeaponPrefabGuid(WeaponType type) const
{
    auto iter = _weaponPoolTable.find(type);
    if (iter != _weaponPoolTable.end())
    {
        return iter->second.PrefabGuid;
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
    auto& gameObjectPool    = _weaponPoolTable[type].GameObjectPool;
    auto& animationPool     = _weaponPoolTable[type].AnimationPool;
    auto& particlePool      = _weaponPoolTable[type].ParticlePool;
    auto& availableIndices  = _weaponPoolTable[type].AvailableIndices;
    if (false == availableIndices.empty())
    {
        size_t index = *availableIndices.begin();

        std::weak_ptr<GameObject>   gameObject = gameObjectPool[index];
        AnimationComponent*         animation  = animationPool[index];
        ParticleComponent*          particle   = particlePool[index];

        WeaponModelData modelData(index, type, gameObject, animation, particle);

        if (auto object = gameObject.lock())
        {
            object->ActiveSelf = true;
            availableIndices.erase(index);
            return modelData;
        }
    }
    return WeaponModelData();
}

bool WeaponModelManager::ReturnWeaponModel(WeaponModelData data)
{
    const WeaponType type  = data.Type;
    const size_t     index = data._index;
    std::set<size_t>& availableIndices = _weaponPoolTable[type].AvailableIndices;
    if (false == availableIndices.contains(index))
    {
        if (auto gameObject = data.GameObject.lock())
        {
            gameObject->ActiveSelf = false;
        }
        availableIndices.insert(index);
        return true;
    }
    return false;
}

bool WeaponModelManager::HasWeaponAnimation(WeaponType type, const std::string& animKey)
{
    auto& animSet = _weaponPoolTable[type].WeaponAnimationKeySet;
    return animSet.contains(animKey);
}

const std::string* WeaponModelManager::GetRandomWeaponAnimationKeyToNormalAttack(WeaponType type)
{
    auto& animList = _weaponPoolTable[type].NormalAnimationKeyList;
    if (false == animList.empty())
    {
        size_t randomIndex = Random::Index(animList.size());
        return &animList[randomIndex];
    }
    return nullptr;
}

const std::string* WeaponModelManager::GetRandomWeaponAnimationKeyToSpecialAttack(WeaponType type)
{
    auto& animList = _weaponPoolTable[type].SpecialAnimationKeyList;
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
    InitializeWeaponPool();
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

            WeaponPool& weaponPool  = _weaponPoolTable[type];
            File::Guid& guid        = weaponPool.PrefabGuid;
            std::string pathStr     = guid.ToPath().string();

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
                        guid = data->GetGuid();
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
            WeaponPool& weaponPool = _weaponPoolTable[type];

            ImGui::PushID(name.data());
            ImGuiHelper::TextWithVerticalSeparator(name.data(), 150.0f);
            ImGui::SameLine();
            size_t availableCount = weaponPool.AvailableIndices.size();
            size_t totalCount     = weaponPool.GameObjectPool.size();
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

void WeaponModelManager::LoadWeaponInstances(WeaponType type, const File::Guid& prefabGuid)
{
    WeaponPool& weaponPool   = _weaponPoolTable[type];
    const auto* weaponPrefab = UmGameObjectFactory.GetOriginPrefab(prefabGuid);
    if (weaponPrefab && weaponPrefab->front())
    {
        auto sharedPrefab = weaponPrefab->front();

        weaponPool.PrefabGuid = prefabGuid;
        weaponPool.GameObjectPool.resize(WEAPON_POOLING_SIZE);
        weaponPool.AnimationPool.resize(WEAPON_POOLING_SIZE);
        weaponPool.ParticlePool.resize(WEAPON_POOLING_SIZE);

        for (size_t i = 0; i < WEAPON_POOLING_SIZE; ++i)
        {
            GameObject* clone = GameObject::Instantiate(sharedPrefab.get());
            if (clone)
            {
                clone->transform->SetParent(transform, false);
                clone->AddTag(rfl::enum_to_string(type));
                clone->SetActive(false);
                weaponPool.GameObjectPool[i]    = clone->GetWeakPtr();
                weaponPool.AnimationPool[i]     = clone->GetComponent<AnimationComponent>();
                weaponPool.ParticlePool[i]      = clone->GetComponent<ParticleComponent>();
                weaponPool.AvailableIndices.insert(i);
            }
        }
    }

    if (false == weaponPool.AnimationPool.empty())
    {
        if (AnimationComponent* anim = weaponPool.AnimationPool.front())
        {
            const auto& keyMap = anim->GetAnimationKeyMap();
            for (auto& [key, anim] : keyMap)
            {
                weaponPool.WeaponAnimationKeySet.insert(key);
                if (key.find("_S_") != std::string::npos)
                { // "_S_" 포함된 키 → 스페셜 리스트
                    weaponPool.SpecialAnimationKeyList.push_back(key);
                }
                else
                { // 나머지 → 일반 리스트
                    weaponPool.NormalAnimationKeyList.push_back(key);
                }
            }
        }
    }
}

void WeaponModelManager::InitializeWeaponPool()
{
    for (auto& [typeStr, guid] : ReflectFields->WeaponPrefabGuidTable)
    {
        auto type = rfl::string_to_enum<WeaponType>(typeStr);
        if (type)
        {
            WeaponType weaponType = type.value();
            File::Guid prefabGuid = File::Guid(guid);
            LoadWeaponInstances(weaponType, prefabGuid);
        }
    }
}