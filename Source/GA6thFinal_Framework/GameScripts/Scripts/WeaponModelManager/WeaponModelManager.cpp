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
                        _weaponAnimationTable[type].push_back(animation);
                    }
                    if (particle)
                    {
                        _weaponParticleTable[type].push_back(particle);
                    }
                    _availableWeaponIndicesTable[type].push(i);
                }
            }
        }
    }
}

void WeaponModelManager::Update() {}

void WeaponModelManager::OnDestroy() {}

void WeaponModelManager::SerializedReflectEvent() {}

void WeaponModelManager::DeserializedReflectEvent()
{
    for (auto& [typeStr, guid] : ReflectFields->WeaponPrefabGuidTable)
    {
        auto type                            = rfl::string_to_enum<WeaponType>(typeStr);
        _weaponPrefabGuidTable[type.value()] = File::Guid(guid);
    }
}

void WeaponModelManager::ImGuiDrawPropertysEvent()
{
    constexpr auto   WeaponTypeArray     = rfl::get_enumerator_array<WeaponType>();
    constexpr size_t WeaponTypeArraySize = WeaponTypeArray.size();
    if (ImGui::TreeNodeEx("Weapon Prefab GUID", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& [name, type] : WeaponTypeArray)
        {
            ImGui::PushID(name.data());

            std::string& guidStr = ReflectFields->WeaponPrefabGuidTable[name.data()];
            File::Guid&  guid    = _weaponPrefabGuidTable[type];
            std::string  pathStr = guid.ToPath().string();
            ImGuiHelper::TextWithVerticalSeparator(name.data(), 150.0f);
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