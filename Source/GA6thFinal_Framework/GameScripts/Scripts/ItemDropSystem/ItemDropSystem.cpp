#include "pchScripts.h"
#include "ItemDropSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ViewModels/ItemDrop/DropArtifacts/DropArtifactsViewModel.h"


//내부 사용 구조체 및 enum
namespace
{
    constexpr auto ArtifactDropTypeArray = rfl::get_enumerator_array<ArtifactDropType>();
    constexpr size_t ArtifactDropTypeArraySize = ArtifactDropTypeArray.size(); // 유물 카테고리 개수

    constexpr auto RevelationGradeArray = rfl::get_enumerator_array<RevelationGrade>();
    constexpr size_t RevelationGradeArraySize = static_cast<size_t>(RevelationGrade::EXTINCTION);

    constexpr auto WeaponTypeArray = rfl::get_enumerator_array<WeaponType>();
    constexpr size_t WeaponTypeArraySize = WeaponTypeArray.size();

    constexpr auto WeaponGradeArray = rfl::get_enumerator_array<WeaponGrade>();
    constexpr size_t WeaponGradeArraySize = WeaponGradeArray.size();

} // namespace

const size_t ItemDropSystem::ARTIFACT_TYPE_COUNT = ArtifactDropTypeArraySize; // 유물 카테고리 개수

ItemDropSystem::ItemDropSystem()
{
    ReflectFields->MaxDropCount = {2, 2, 2, 6, 7, 2};
}
ItemDropSystem::~ItemDropSystem()
{
    UmWatcher.Unregister<DropArtifactsViewModel>(ItemDropSystem::WATCHER_KEY);
}

std::array<DropItemInfo, ARTIFACT_DROP_COUNT> ItemDropSystem::RollArtifacts()
{  
    RevelationSystem* revelationSystem = RevelationSystem::GetInstance();
    std::array<std::vector<RevelationElement*>, RevelationGradeArraySize> revelations{};
    if (revelationSystem)
    {
        // 등급별 계시 분류
        const auto& table = revelationSystem->GetRevelationTableElements();
        for (auto& revelation : table)
        {
            RevelationGrade grade = revelation->Grade;
            size_t          index = static_cast<size_t>(grade);
            revelations[index].push_back(revelation);
        }      
    }

    //무기 테이블에 대한 이중 배열. 첫번째는 무기 타입, 두번째는 무기 등급
    std::array<std::array<std::vector<WeaponElement*>, WeaponGradeArraySize>, WeaponTypeArraySize> weapons{};
    WeaponTableComponent* weaponTableComponent = WeaponTableComponent::GetInstance();
    if (weaponTableComponent)
    {
        // 등급별 무기 분류
        const auto& table = weaponTableComponent->GetWeaponTableElements();
        for (auto& weapon : table)
        {
            WeaponType  type       = weapon->Stats.Type;
            size_t      typeIndex  = static_cast<size_t>(type);
            WeaponGrade grade      = weapon->Stats.Grade;
            size_t      gradeIndex = static_cast<size_t>(grade);
            weapons[typeIndex][gradeIndex].push_back(weapon);
        }
    }

    std::array<DropItemInfo, ARTIFACT_DROP_COUNT> artifacts;      // 결과 담는 배열
    std::array<int, ArtifactDropTypeArraySize>    maxDropCount{}; // 중복 등장 카운트용
    for (size_t i = 0; i < maxDropCount.size(); ++i)
    {
        maxDropCount[i] = ReflectFields->MaxDropCount[i];
    }
    std::vector<ArtifactDropType> dropCategory;                  // 이번에 등장 가능한 분류
    for (auto& artifact : artifacts)
    {        
        // 등장 가능한 종류 등록
        dropCategory.clear();
        for (size_t i = 0; i < maxDropCount.size(); ++i)
        {
            int count = maxDropCount[i];
            if (0 < count)
            {
                auto& [str, value] = ArtifactDropTypeArray[i];
                dropCategory.push_back(value);
            }
        }

        // 랜덤한 종류 뽑기
        size_t randomCategoryIndex = Random::Index(dropCategory.size());
        --maxDropCount[randomCategoryIndex];
        ArtifactDropType type = dropCategory[randomCategoryIndex];

        //종류별 랜덤 뽑기
        auto RollWeaponRandomGrade =[this](const std::array<double, WeaponGradeArraySize>& weight) -> WeaponGrade 
        { 
            auto& [gradeStr, grade] = WeaponGradeArray[Random::Index(weight)];
            return grade;
        };
        auto RollRevelationRandomGrade =[this](const std::array<double, RevelationGradeArraySize>& weight) -> RevelationGrade
        {
            auto& [gradeStr, grade] = RevelationGradeArray[Random::Index(weight)];
            return grade;
        };

        //임시 가중치 추후 변경 필요.
        std::array<double, WeaponGradeArraySize> weaponTempWeight{};
        weaponTempWeight.fill(1.0 / WeaponGradeArraySize);
        std::array<double, RevelationGradeArraySize> revelationTempWeight{};
        revelationTempWeight.fill(1.0 / RevelationGradeArraySize);

        // 무기 랜덤 뽑기 함수
        auto RollWeaponRandomItem = [&](int type) -> DropItemInfo 
        {
            WeaponGrade grade      = RollWeaponRandomGrade(weaponTempWeight);
            int         gradeIndex = static_cast<int>(grade);
            auto&       itemTable  = weapons[type][gradeIndex];
            if (false == itemTable.empty())
            {
                size_t     itemIndex  = Random::Index(itemTable.size());
                IDropItem* randomItem = itemTable[itemIndex];
                itemTable.erase(itemTable.begin() + itemIndex);
                return randomItem->GetItemInfo();
            }
            else
            {
                DropItemInfo info{};
                info.ID         = 0;
                info.CategoryID = DropItemInfo::GetArtifactCategoryAssetID(static_cast<ArtifactDropType>(type));
                info.Name       = rfl::enum_to_string(grade);
                info.Name += (const char*)u8" 등급 ";
                info.Name += rfl::enum_to_string(static_cast<WeaponType>(type));
                return info;
            }
        };
        // 계시 랜덤 뽑기 함수
        auto RollRevelationRandomItem = [&]() -> DropItemInfo
        {
            RevelationGrade grade      = RollRevelationRandomGrade(revelationTempWeight);
            int             gradeIndex = static_cast<int>(grade);
            auto&           itemTable  = revelations[gradeIndex];
            if (false == itemTable.empty())
            {
                size_t     itemIndex  = Random::Index(itemTable.size());
                IDropItem* randomItem = itemTable[itemIndex];
                itemTable.erase(itemTable.begin() + itemIndex);
                return randomItem->GetItemInfo();
            }
            else
            {
                DropItemInfo info{};
                info.ID         = 0;
                info.CategoryID = DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType::REVELATION);
                info.Name       = rfl::enum_to_string(grade);
                info.Name       += (const char*)u8" 등급 계시";
                return info;
            }
        };

        switch (type)
        {
        case ArtifactDropType::SWORD:
            artifact = RollWeaponRandomItem(static_cast<int>(WeaponType::SWORD));
            break;
        case ArtifactDropType::DAGGER:
            artifact = RollWeaponRandomItem(static_cast<int>(WeaponType::DAGGER));
            break;
        case ArtifactDropType::WARHAMMER:
            artifact = RollWeaponRandomItem(static_cast<int>(WeaponType::WARHAMMER));
            break;
        case ArtifactDropType::REVELATION:
            artifact = RollRevelationRandomItem();
            break;
        case ArtifactDropType::ERASE_REVELATION:
            artifact.ID         = DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType::ERASE_REVELATION);
            artifact.CategoryID = DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType::ERASE_REVELATION);
            artifact.Name       = (const char*)u8"계시 지우기 (테스트)";
            break;
        default:
            break;
        }      
    }
    return artifacts;
}

void ItemDropSystem::SetDropItem(const std::array<DropItemInfo, ARTIFACT_DROP_COUNT>& itemInfos)
{
    std::vector<DropItemInfo> dropItems(itemInfos.begin(), itemInfos.end());
    _dropItemsModel = dropItems;
}

void ItemDropSystem::SetStageClearCount(int count) 
{
    _stageClearCount = std::clamp(count, 0, 3);

}

void ItemDropSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNode("Artifacts"))
    {
        ImGuiDrawMaxDropCount();
        ImGuiDrawTestRollArtifacts();   
        ImGui::TreePop();
    }
}

void ItemDropSystem::ImGuiDrawMaxDropCount() 
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"한 스테이지에 카테고리별 드롭 횟수를 조절합니다."); };
    if (ImGui::TreeNode("Max Drop Count"))
    {
        TreeToolTip();
        int i = 0;
        auto& MaxDropCount = ReflectFields->MaxDropCount;
        for (auto& [str, value] : ArtifactDropTypeArray)
        {
            if (i < MaxDropCount.size())
            {
                if (ImGui::DragInt(str.data(), &MaxDropCount[i]))
                {
                    MaxDropCount[i] = std::clamp(MaxDropCount[i], 0, 99);
                }
            }
            i++;
        }
        ImGui::TreePop();
    }
    else
    {
        TreeToolTip();
    }
}

void ItemDropSystem::ImGuiDrawTestRollArtifacts() 
{
    auto TreeToolTip = [](){ ImGuiHelper::HoveredToolTip(u8"아이템 보상 뽑기를 테스트합니다."); };
    if (ImGui::TreeNode("Test Artifacts Drop"))
    {
        TreeToolTip();
        if (ImGui::Button("Roll Artifacts"))
        {
            std::array<DropItemInfo, ARTIFACT_DROP_COUNT> artifacts = RollArtifacts();
            if (ArtifactUIManager* uiManager = ArtifactUIManager::GetInstance())
            {
                uiManager->UpdateImageElements(std::vector<DropItemInfo>(artifacts.begin(), artifacts.end()));
            }
        }

        const auto& model = _dropItemsModel;
        for (auto& item : model)
        {
            const DropItemInfo& info = item;
            ImGui::PushID(&info);
            ImGui::Selectable(info.Name.data());
            ImGui::PopID();
        }    
        ImGui::TreePop();
    }
    else
    {
        TreeToolTip();
    }
}

void ItemDropSystem::SerializedReflectEvent() 
{

}

void ItemDropSystem::DeserializedReflectEvent() 
{

}

void ItemDropSystem::Reset() 
{
    ReflectFields->MaxDropCount.resize(ArtifactDropTypeArraySize);
}

void ItemDropSystem::Awake()
{
    static_instance = this;
    UmWatcher.Register<DropArtifactsViewModel>(ItemDropSystem::WATCHER_KEY, _dropItemsModel);
}

 