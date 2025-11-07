#include "pchScripts.h"
#include "ItemDropSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "WeaponSystem/WeaponTable/WeaponTableComponent.h"
#include "RevelationSystem/RevelationSystem.h"
#include "AccessorySystem/AccessorySystem.h"
#include "ItemDropSystem/UI/ItemDropUIRootManager.h"
#include "ItemDropSystem/UI/ArtifactUIManager.h"
#include "ItemDropSystem/UINavi/ArtifactButtonNavi.h"
#include "ViewModels/ItemDrop/DropArtifacts/DropArtifactsViewModel.h"
#include "Debugger/Debugger.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"
#include "TutorialSystem/TutorialSystem.h"

#include "Map/MapManager.h"
#include "Map/Stage.h"
#include "CombatUIManager/CombatUIManager.h"
#include "Audio/BGMManager.h"

UMREAL_COMPONENT(ItemDropSystem)

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

    constexpr auto AccessoryGradeArray = rfl::get_enumerator_array<AccessoryGrade>();
    constexpr size_t AccessoryGradeArraySize = AccessoryGradeArray.size();

    /// <summary>
    /// 유효한 가중치 배열로 설정합니다. (total 값이 0 이상으로)
    /// </summary>
    /// <param name="weights"></param>
    void ResetValidWeights(std::vector<double>& weights) 
    {
        if (false == weights.empty())
        {
            double total = 0.0;
            for (auto& weight : weights)
            {
                total += weight;
            }

            if (total < static_cast<double>(Mathf::Epsilon))
            {
                std::fill(weights.begin(), weights.end(), 1.0 / static_cast<double>(weights.size()));
            }
        }
    }

} // namespace

const size_t ItemDropSystem::ARTIFACT_TYPE_COUNT = ArtifactDropTypeArraySize; // 유물 카테고리 개수

ItemDropSystem::ItemDropSystem()
{
    _obtainArtifactFlag.fill(0);

    ReflectFields->MaxDropCount = {2, 2, 2, 6, 7, 2};

    for (auto& weights : ReflectFields->WeaponGradeWeight)
    {
        weights.resize(WeaponGradeArraySize, 1.0 / (double)WeaponGradeArraySize);
    }

    for (auto& weights : ReflectFields->RevelationGradeWeight)
    {
        weights.resize(RevelationGradeArraySize, 1.0 / (double)RevelationGradeArraySize);
    }

    for (auto& weights : ReflectFields->AccessoryGradeWeight)
    {
        weights.resize(AccessoryGradeArraySize, 1.0 / (double)AccessoryGradeArraySize);
    }
}
ItemDropSystem::~ItemDropSystem() = default;


std::array<DropItemInfo, ARTIFACT_DROP_COUNT> ItemDropSystem::RollArtifacts()
{  
    RevelationSystem* revelationSystem = SingletonComponent<RevelationSystem>::GetInstance();
    std::array<std::vector<RevelationElement*>, RevelationGradeArraySize> revelations{};
    if (revelationSystem)
    {
        // 등급별 계시 분류
        const auto& table = revelationSystem->GetRevelationTableElements();
        for (auto& revelation : table)
        {
            RevelationGrade grade = revelation->Grade;
            if (RevelationGrade::EXTINCTION != grade)
            {
                size_t index = static_cast<size_t>(grade);
                revelations[index].push_back(revelation);
            }
        }      
    }

    //무기 테이블에 대한 이중 배열. 첫번째는 무기 타입, 두번째는 무기 등급
    std::array<std::array<std::vector<WeaponElement*>, WeaponGradeArraySize>, WeaponTypeArraySize> weapons{};
    WeaponTableComponent* weaponTableComponent = SingletonComponent<WeaponTableComponent>::GetInstance();
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

    //장신구 테이블에 대한 배열.
    std::array<std::vector<AccessoryElement*>, AccessoryGradeArraySize> accessories{};
    AccessorySystem* accessorySystem = SingletonComponent<AccessorySystem>::GetInstance();
    if (accessorySystem)
    {
        const auto& table = accessorySystem->GetAccessoryTableElements();
        for (auto& accessory : table)
        {
            //중복 장신구는 제외
            if (false == accessorySystem->HasPlayerAccessory(*accessory))
            {
                AccessoryGrade grade     = accessory->Grade;
                size_t         typeIndex = static_cast<size_t>(grade);

                accessories[typeIndex].push_back(accessory);
            }
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
        auto RollWeaponRandomGrade = [this](const std::vector<double>& weight) -> WeaponGrade 
        { 
            auto& [gradeStr, grade] = WeaponGradeArray[Random::Index(weight)];
            return grade;
        };
        auto RollRevelationRandomGrade = [this](const std::vector<double>& weight) -> RevelationGrade
        {
            auto& [gradeStr, grade] = RevelationGradeArray[Random::Index(weight)];
            return grade;
        };
        auto RollAccessoryRandomGrade = [this](const std::vector<double>& weight) -> AccessoryGrade
        {
            auto& [gradeStr, grade] = AccessoryGradeArray[Random::Index(weight)];
            return grade;
        };

        // 무기 랜덤 뽑기 함수
        auto RollWeaponRandomItem = [&](int type) -> DropItemInfo 
        {
            WeaponGrade grade      = RollWeaponRandomGrade(ReflectFields->WeaponGradeWeight[_itemDropRateBonus]);
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
                info.Category = static_cast<ArtifactDropType>(type);
                info.Name       = rfl::enum_to_string(grade);
                info.Name += (const char*)u8" 등급 ";
                info.Name += rfl::enum_to_string(static_cast<WeaponType>(type));
                return info;
            }
        };
        // 계시 랜덤 뽑기 함수
        auto RollRevelationRandomItem = [&]() -> DropItemInfo
        {
            RevelationGrade grade      = RollRevelationRandomGrade(ReflectFields->RevelationGradeWeight[_itemDropRateBonus]);
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
                info.Category = ArtifactDropType::REVELATION;
                info.Name       = rfl::enum_to_string(grade);
                info.Name       += (const char*)u8" 등급 계시";
                return info;
            }
        };
        // 장신구 랜덤 뽑기 함수
        auto RollAccessoryRandomItem = [&]() -> DropItemInfo
        {
            AccessoryGrade grade = RollAccessoryRandomGrade(ReflectFields->AccessoryGradeWeight[_itemDropRateBonus]);
            int            gradeIndex = static_cast<int>(grade);
            auto&          itemTable  = accessories[gradeIndex];
            if (false == itemTable.empty())
            {
                size_t itemIndex = Random::Index(itemTable.size());
                IDropItem* randomItem = itemTable[itemIndex];
                itemTable.erase(itemTable.begin() + itemIndex);
                return randomItem->GetItemInfo();
            }
            else
            {
                DropItemInfo info{};
                info.ID       = 0;
                info.Category = ArtifactDropType::ACCESSORY;
                info.Name     = rfl::enum_to_string(grade);
                info.Name += (const char*)u8" 등급 장신구";
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
        case ArtifactDropType::ACCESSORY:
            artifact = RollAccessoryRandomItem();
            break;
        case ArtifactDropType::REVELATION:
            artifact = RollRevelationRandomItem();
            break;
        case ArtifactDropType::ERASE_REVELATION:
            artifact.ID       = DropItemInfo::GetArtifactCategoryAssetID(ArtifactDropType::ERASE_REVELATION, false);
            artifact.Category = ArtifactDropType::ERASE_REVELATION;
            artifact.Name     = (const char*)u8"계시 지우기";
            break;
        default:
            break;
        }      
    }
    return artifacts;
}

void ItemDropSystem::RollArtifactsCurrent() 
{
    if (_dropItemsModel.empty())
    {
        SetDropItem(RollArtifacts());
        return;
    }

    RevelationSystem* revelationSystem = SingletonComponent<RevelationSystem>::GetInstance();
    std::array<std::vector<RevelationElement*>, RevelationGradeArraySize> revelations{};
    if (revelationSystem)
    {
        // 등급별 계시 분류
        const auto& table = revelationSystem->GetRevelationTableElements();
        for (auto& revelation : table)
        {
            RevelationGrade grade = revelation->Grade;
            if (RevelationGrade::EXTINCTION != grade)
            {
                size_t index = static_cast<size_t>(grade);
                revelations[index].push_back(revelation);
            }
        }
    }

    // 무기 테이블에 대한 이중 배열. 첫번째는 무기 타입, 두번째는 무기 등급
    std::array<std::array<std::vector<WeaponElement*>, WeaponGradeArraySize>, WeaponTypeArraySize> weapons{};
    WeaponTableComponent* weaponTableComponent = SingletonComponent<WeaponTableComponent>::GetInstance();
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

    // 장신구 테이블에 대한 배열.
    std::array<std::vector<AccessoryElement*>, AccessoryGradeArraySize> accessories{};
    AccessorySystem* accessorySystem = SingletonComponent<AccessorySystem>::GetInstance();
    if (accessorySystem)
    {
        const auto& table = accessorySystem->GetAccessoryTableElements();
        for (auto& accessory : table)
        {
            // 중복 장신구는 제외
            if (false == accessorySystem->HasPlayerAccessory(*accessory))
            {
                AccessoryGrade grade     = accessory->Grade;
                size_t         typeIndex = static_cast<size_t>(grade);

                accessories[typeIndex].push_back(accessory);
            }
        }
    }

    // 카테고리 유지하고 뽑기
    size_t index = 0;
    _dropItemsModel.for_each([&](DropItemInfo& artifact) 
    {
        // 현재 종류
        ArtifactDropType type = artifact.Category;

        // 종류별 랜덤 뽑기
        auto RollWeaponRandomGrade = [this](const std::vector<double>& weight) -> WeaponGrade {
            auto& [gradeStr, grade] = WeaponGradeArray[Random::Index(weight)];
            return grade;
        };
        auto RollRevelationRandomGrade = [this](const std::vector<double>& weight) -> RevelationGrade {
            auto& [gradeStr, grade] = RevelationGradeArray[Random::Index(weight)];
            return grade;
        };
        auto RollAccessoryRandomGrade = [this](const std::vector<double>& weight) -> AccessoryGrade {
            auto& [gradeStr, grade] = AccessoryGradeArray[Random::Index(weight)];
            return grade;
        };

        // 무기 랜덤 뽑기 함수
        auto RollWeaponRandomItem = [&](int type) -> DropItemInfo {
            WeaponGrade grade      = RollWeaponRandomGrade(ReflectFields->WeaponGradeWeight[_itemDropRateBonus]);
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
                info.ID       = 0;
                info.Category = static_cast<ArtifactDropType>(type);
                info.Name     = rfl::enum_to_string(grade);
                info.Name += (const char*)u8" 등급 ";
                info.Name += rfl::enum_to_string(static_cast<WeaponType>(type));
                return info;
            }
        };
        // 계시 랜덤 뽑기 함수
        auto RollRevelationRandomItem = [&]() -> DropItemInfo {
            RevelationGrade grade = RollRevelationRandomGrade(ReflectFields->RevelationGradeWeight[_itemDropRateBonus]);
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
                info.ID       = 0;
                info.Category = ArtifactDropType::REVELATION;
                info.Name     = rfl::enum_to_string(grade);
                info.Name += (const char*)u8" 등급 계시";
                return info;
            }
        };
        // 장신구 랜덤 뽑기 함수
        auto RollAccessoryRandomItem = [&]() -> DropItemInfo {
            AccessoryGrade grade = RollAccessoryRandomGrade(ReflectFields->AccessoryGradeWeight[_itemDropRateBonus]);
            int            gradeIndex = static_cast<int>(grade);
            auto&          itemTable  = accessories[gradeIndex];
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
                info.ID       = 0;
                info.Category = ArtifactDropType::ACCESSORY;
                info.Name     = rfl::enum_to_string(grade);
                info.Name += (const char*)u8" 등급 장신구";
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
        case ArtifactDropType::ACCESSORY:
            artifact = RollAccessoryRandomItem();
            break;
        case ArtifactDropType::REVELATION:
            artifact = RollRevelationRandomItem();
            break;
        case ArtifactDropType::ERASE_REVELATION:
            break;
        default:
            break;
        }
        ++index;
    });  
}

void ItemDropSystem::SetDropItem(const std::array<DropItemInfo, ARTIFACT_DROP_COUNT>& itemInfos)
{
    std::vector<DropItemInfo> dropItems(itemInfos.begin(), itemInfos.end());
    _dropItemsModel = dropItems;
    _obtainArtifactFlag.fill(0);
}

void ItemDropSystem::SetStageClearCount(int count) 
{
    _stageClearCount = std::clamp(count, 0, 3);
    if (ArtifactUIManager* uiManager = SingletonComponent<ArtifactUIManager>::GetInstance())
    {
        uiManager->UpdateUnlock();
    }
    if (MapManager* manager = SingletonComponent<MapManager>::GetInstance())
    {
        if (Stage* stage = manager->GetCurrentSelectedStage())
        {
            // n번 째 전투 = 클리어 횟수의 +1
            stage->BattleCount = count + 1;
        }
    }
}

void ItemDropSystem::PlayItemDropUISequence() 
{
#ifdef _UMEDITOR
    if (false == UmCore->IsPlay())
    {
        return;
    }
#endif 

    if (ItemDropUIRootManager* itemDropUIRootManager = SingletonComponent<ItemDropUIRootManager>::GetInstance())
    {
        itemDropUIRootManager->gameObject->ActiveSelf = true;
        if (FadeUIComponent* fadeUI = itemDropUIRootManager->GetComponent<FadeUIComponent>())
        {
            fadeUI->Begin();
            fadeUI->FadeIn();
        }
        StageClearCount = StageClearCount + 1;
        ++WinCount;

        if (ArtifactUIManager* manager = SingletonComponent<ArtifactUIManager>::GetInstance())
        {
            // 플래그 초기화
            manager->ResetObtainFlag();

            // 보상 설정이 안되어있으면 자동으로 뽑는다.
            if (_dropItemsModel.empty())
            {
                std::array<DropItemInfo, ARTIFACT_DROP_COUNT> artifacts = RollArtifacts();
                SetDropItem(artifacts);
            }

            // 첫번째 클리어일때는 뽑기 실행
            if (StageClearCount == 1)
            {
                RollArtifactsCurrent();
            }

            // 버튼 기능 설정
            const auto& dropItemInfos = _dropItemsModel;
            size_t      i             = 0;
            for (const auto& itemInfo : dropItemInfos)
            {
                manager->SetNaviDropItemInfo(itemInfo, i);
                ++i;
            }

            // 포커스 되야할 버튼
            ArtifactButtonNavi::LastFocusIndex = 0;
            itemDropUIRootManager->UpdateStory();
            UmTime.Invoke(itemDropUIRootManager, 0.1f, [itemDropUIRootManager]() 
            { 
                itemDropUIRootManager->AutoFocus();
            });
           
            if (BGMManager* bgmManager = SingletonComponent<BGMManager>::GetInstance())
            {
                bgmManager->PlayBGM("-460000", true);
            }

            // UI 갱신
            _dropItemsModel.Notify();

            // 튜토리얼
            if (TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance())
            {
                system->Show(805914); //보상과 추가 전투
            }
        }
    }

    if (CombatUIManager* uiManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        uiManager->TurnQueueGroup.ActiveUI(false);
        uiManager->CharacterHUDGroup.ActiveUI(false);
        uiManager->RevelationsGroup.ActiveUI(false);
        uiManager->WeaponGroup.ActiveUI(false);
    }
}

void ItemDropSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Play Item Drop UI Sequence"))
    {
        PlayItemDropUISequence();
    }
    ImGuiHelper::HoveredToolTip((const char*)u8"플레이 모드에서만 동작합니다.");

    if (ImGui::TreeNode("Artifacts"))
    {
        ImGuiDrawTestRollArtifacts();   
        ImGuiDrawMaxDropCount();
        ImGuiDrawWeaponGradeWeight();
        ImGuiDrawRevelationGradeWeight();
        ImGuiDrawAccessoryGradeWeight();
        ImGui::TreePop();
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
            SetDropItem(artifacts);
            if (ArtifactUIManager* uiManager = SingletonComponent<ArtifactUIManager>::GetInstance())
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

void ItemDropSystem::ImGuiDrawMaxDropCount()
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"한 스테이지에 카테고리별 드롭 횟수를 조절합니다."); };
    if (ImGui::TreeNode("Max Drop Count"))
    {
        TreeToolTip();
        int   i            = 0;
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

void ItemDropSystem::ImGuiDrawWeaponGradeWeight()
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"무기 등급별 드랍 확률을 조절합니다."); };
    if (ImGui::TreeNode("Weapon Grade Weight"))
    {
        TreeToolTip();
        if (ImGui::BeginTable("Weapon Grade Weight", WeaponGradeArraySize + 2,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Rate Bonus");
            for (auto& [str, value] : WeaponGradeArray)
            {
                ImGui::TableSetupColumn(str.data());
            }
            ImGui::TableSetupColumn("Total");
            ImGui::TableHeadersRow();

            int row = 0;
            for (auto& weights : ReflectFields->WeaponGradeWeight)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Selectable(std::to_string(row).c_str());
                double totalPercent = 0.0;
                size_t i = 1;
                for (; i <= weights.size(); ++i)
                {
                    ImGui::TableSetColumnIndex((int)i);
                    double& weight = weights[i - 1];
                    ImGui::PushID(&weight);
                    {
                        double       weightPercent = weight * 100.0;
                        const double min           = 1.0;
                        const double max           = 100.0;
                        if (ImGui::DragScalar("##inputDouble", ImGuiDataType_Double, &weightPercent, 1.f, &min, &max,
                                              "%.3f"))
                        {
                            weight = weightPercent / 100.0;
                        }
                        totalPercent += weightPercent;
                    }
                    ImGui::PopID();
                }
                ImGui::TableSetColumnIndex((int)i);
                ImGui::Selectable(std::format("{:.3f}", totalPercent).c_str());
                ++row;
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
    else
    {
        TreeToolTip();
    }
}

void ItemDropSystem::ImGuiDrawRevelationGradeWeight() 
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"계시 등급별 드랍 확률을 조절합니다."); };
    if (ImGui::TreeNode("Revelation Grade Weight"))
    {
        TreeToolTip();
        if (ImGui::BeginTable("Revelation Grade Weight", RevelationGradeArraySize + 2,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Rate Bonus");
            for (size_t i = 0; i < RevelationGradeArraySize; ++i)
            {
                auto& [str, value] = RevelationGradeArray[i];
                ImGui::TableSetupColumn(str.data());
            }
            ImGui::TableSetupColumn("Total");
            ImGui::TableHeadersRow();

            int row = 0;
            for (auto& weights : ReflectFields->RevelationGradeWeight)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Selectable(std::to_string(row).c_str());
                double totalPercent = 0.0;
                size_t i = 1;
                for (; i <= weights.size(); ++i)
                {
                    ImGui::TableSetColumnIndex((int)i);
                    double& weight = weights[i - 1];
                    ImGui::PushID(&weight);
                    {
                        double       weightPercent = weight * 100.0;
                        const double min           = 1.0;
                        const double max           = 100.0;
                        if (ImGui::DragScalar("##inputDouble", ImGuiDataType_Double, &weightPercent, 1.f, &min, &max,
                                              "%.3f"))
                        {
                            weight = weightPercent / 100.0;
                        }
                        totalPercent += weightPercent;
                    }
                    ImGui::PopID();
                }
                ImGui::TableSetColumnIndex((int)i);
                ImGui::Selectable(std::format("{:.3f}", totalPercent).c_str());
                ++row;
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
    else
    {
        TreeToolTip();
    }
}

void ItemDropSystem::ImGuiDrawAccessoryGradeWeight() 
{
    auto TreeToolTip = []() { ImGuiHelper::HoveredToolTip(u8"장신구 등급별 드랍 확률을 조절합니다."); };
    if (ImGui::TreeNode("Accessory Grade Weight"))
    {
        TreeToolTip();
        if (ImGui::BeginTable("Accessory Grade Weight", AccessoryGradeArraySize + 2,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Rate Bonus");
            for (size_t i = 0; i < AccessoryGradeArraySize; ++i)
            {
                auto& [str, value] = AccessoryGradeArray[i];
                ImGui::TableSetupColumn(str.data());
            }
            ImGui::TableSetupColumn("Total");
            ImGui::TableHeadersRow();

            int row = 0;
            for (auto& weights : ReflectFields->AccessoryGradeWeight)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Selectable(std::to_string(row).c_str());
                double totalPercent = 0.0;
                size_t i            = 1;
                for (; i <= weights.size(); ++i)
                {
                    ImGui::TableSetColumnIndex((int)i);
                    double& weight = weights[i - 1];
                    ImGui::PushID(&weight);
                    {
                        double       weightPercent = weight * 100.0;
                        const double min           = 1.0;
                        const double max           = 100.0;
                        if (ImGui::DragScalar("##inputDouble", ImGuiDataType_Double, &weightPercent, 1.f, &min, &max,
                                              "%.3f"))
                        {
                            weight = weightPercent / 100.0;
                        }
                        totalPercent += weightPercent;
                    }
                    ImGui::PopID();
                }
                ImGui::TableSetColumnIndex((int)i);
                ImGui::Selectable(std::format("{:.3f}", totalPercent).c_str());
                ++row;
            }
            ImGui::EndTable();
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
    for (auto& weights : ReflectFields->WeaponGradeWeight)
    {
        weights.resize(WeaponGradeArraySize);
        ResetValidWeights(weights);
    }
   
    for (auto& weights : ReflectFields->RevelationGradeWeight)
    {
        weights.resize(RevelationGradeArraySize);
        ResetValidWeights(weights);
    }

    for (auto& weights : ReflectFields->AccessoryGradeWeight)
    {
        weights.resize(AccessoryGradeArraySize);
        ResetValidWeights(weights);
    }
}

void ItemDropSystem::Reset() 
{
    _singletonComponent.SetSingleTon();
    ReflectFields->MaxDropCount.resize(ArtifactDropTypeArraySize);
}

void ItemDropSystem::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
        UmWatcher.Register<DropArtifactsViewModel>(ItemDropSystem::WATCHER_KEY, _dropItemsModel);
    }   
}

void ItemDropSystem::Update() 
{
    Debugger debuger;  
    debuger([this]() { DebugUpdate(); });
}

void ItemDropSystem::OnDestroy() 
{
    if (_singletonComponent.IsSingleTon())
    {
        UmWatcher.Unregister<DropArtifactsViewModel>(ItemDropSystem::WATCHER_KEY);
    }
}

void ItemDropSystem::DebugUpdate() 
{
    using namespace u8_literals;
    ImGui::Separator();
    if (ImGui::TreeNode(u8"Item Drop System"_c_str))
    {
        ImGuiDrawPropertys();
        ImGui::TreePop();
    }
}

 