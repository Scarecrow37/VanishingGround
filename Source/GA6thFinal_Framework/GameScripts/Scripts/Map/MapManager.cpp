#include "pchScripts.h"
#include "MapManager.h"
#include "Map/RewardPopup.h"
#include "Map/SmoothScroll.h"
#include "Map/Stage.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Views/Map/StageFocusView.h"
#include "UI/Views/Map/StageView.h"
#include "UI/Wrappers/Scrolling/ScrollingWrapper.h"
#include "ViewModels/Map/StageFocusViewModel.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "ViewModels/Hp/CharacterHPViewModel.h"
#include "Utility/SingletonHelper.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "Preferences/PreferencesManager.h"
#include "Inventory/UI/InventoryUIManager.h"

#include "DifficultyManager/DifficultyEnum.h"

UMREAL_COMPONENT(MapManager)

MapManager::MapManager()
{
    MapScenePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".UmScene")
                {
                    ReflectFields->MapScenePath = UmFileSystem.GetGuidFromPath(path).string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    BackgroundImage.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".png")
                {
                    ReflectFields->AssetIDs[BACKGROUND] = UmFileSystem.GetAssetIDFromGuid(data->GetGuid());
                    ChageBackgroundImage(ReflectFields->AssetIDs[BACKGROUND]);
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    auto PayLoadEvent = [this](int index, int assetID) {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".png")
                {
                    ReflectFields->AssetIDs[index] = UmFileSystem.GetAssetIDFromGuid(data->GetGuid());
                }
            }
            ImGui::EndDragDropTarget();
        }
    };

    StageEnableImage.SetInputAutoEvent([this, PayLoadEvent]() { PayLoadEvent(STAGE_ENABLE, ReflectFields->AssetIDs[STAGE_ENABLE]); });
    StageDisableImage.SetInputAutoEvent([this, PayLoadEvent]() { PayLoadEvent(STAGE_DISABLE, ReflectFields->AssetIDs[STAGE_DISABLE]); });
    StageFocusImage.SetInputAutoEvent([this, PayLoadEvent]() { PayLoadEvent(STAGE_FOCUS, ReflectFields->AssetIDs[STAGE_FOCUS]); });
    RewardPopupImage.SetInputAutoEvent([this, PayLoadEvent]() { PayLoadEvent(REWARD_POPUP, ReflectFields->AssetIDs[REWARD_POPUP]); });
}

MapManager::~MapManager()
{
    if (_singletonObject.IsSingleTon())
    {
        UmWatcher.Unregister<StageFocusViewModel>("StageFocus");
    }
}

void MapManager::SetFocusStage(Stage* stage)
{
    if (stage)
    {
        _focusStage = stage;
    }
}

bool MapManager::TrySelectStage(Stage* stage)
{
    // 동일한 메인 레벨 스테이지들 비활성화
    if (stage && stage->IsEnable())
    {
        // 현재 클리어된 스테이지보다 1단계 높은 스테이지만 선택 가능
        if (stage->MainLevel == _lastClearedStageData.MainLevel+ 1)
        {
            SetSelectStage(stage);
            return true;
        }
    }
    return false;
}

void MapManager::SetSelectStage(Stage* stage) 
{
    if (stage)
    {
        stage->OnSelected();
        _selectedStage = stage;
        _lastClearedStageData.MainLevel = stage->MainLevel;
        _lastClearedStageData.SubLevel  = stage->SubLevel;
    }
}

void MapManager::Awake()
{    
    if (_singletonObject.TrySingleTon(true))
    {        
        _singletonComponent.TrySingleTon();

        UmWatcher.Register<StageFocusViewModel>("StageFocus", _focusStage);
        SetupStage();

        BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &MapManager::PreferencesKeyDown);
        BindInputAction(ControllerButton::START, Action::PRESSED, this, &MapManager::InventoryKeyDown);
        BindInputAction(ControllerButton::RIGHT_THUMB_STICK, Action::HELD, this, &MapManager::ScrollKeyUpdate);
    }
}

void MapManager::Update()
{
    if (_scroll)
    {
        if (Mathf::Epsilon < _scrollDir || _scrollDir < -Mathf::Epsilon)
        {
            _scroll->Scroll += _scrollDir * UmTime.DeltaTime();
            _scrollDir = 0.f;
        }
    }

    if (_openPreferences)
    {
        if (PreferencesManager* manager = SingletonComponent<PreferencesManager>::GetInstance())
        {
            manager->OnPreferencesWindow(_lastFocusStage);
        }  
        _lastFocusStage = nullptr;
        _openPreferences = false;
    }

    if (_openInventory)
    {
        if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
        {
            manager->OpenInventory(_lastFocusStage);
        }
        _lastFocusStage = nullptr;
        _openInventory  = false;
    }

    Debugger()([this]{
        // 아래는 디버그용 코드입니다.
        ImGuiHelper::AlignedText("Map Select", ImGuiHelper::LEFT, 0.8f);
        char curHeader = '0';
        for (const auto& stage : _stages)
        {
            if (stage)
            {
                const std::string& stageName = stage->gameObject->Name;
                if (stageName.length() > 6) {
                    char thisHeader = stageName.at(6);
                    if (curHeader == thisHeader)
                    {
                        ImGui::SameLine();
                    }
                    curHeader = thisHeader;
                }
                if (ImGui::Button(stageName.c_str()))
                {
                    SetSelectStage(stage);
                }
            }
        }
        ImGui::Separator();
        if (ImGui::Button("Preferences"))
        {
            OpenPreferencesWindow();
        }
    });
}

void MapManager::OnEnable() 
{
    UpdateStageFocus();
}

void MapManager::OnLoadScene(Scene& loadScene, LoadSceneMode mode)
{
    std::string otherScene = loadScene.Path;
    bool        isActive   = true;

    if (UmFileSystem.GetPathFromGuid(ReflectFields->MapScenePath).string() == otherScene)
    {
        isActive = true;
    }
    else
    {
        isActive = false;
        if (Transform* preferences = transform->Find("PreferencesPannel"))
        {
            GameObject::Destroy(preferences->gameObject);
        }
        if (Transform* inventory = transform->Find("Inventory Panel"))
        {
            GameObject::Destroy(inventory->gameObject);
        }
    }
    gameObject->SetActive(isActive);
}

void MapManager::ImGuiDrawPropertysEvent()
{    
    ImGui::Separator();
    ImGuiHelper::AlignedText("Stage Info", ImGuiHelper::LEFT, 0.8f);
    ImGui::Text("Cleared Stage Data: %i, %i", _lastClearedStageData.MainLevel, _lastClearedStageData.SubLevel);
    ImGui::Text("Current Stage: ");
    if (_selectedStage)
    {
        ImGui::BeginDisabled();
        _selectedStage->ImGuiDrawPropertys();
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    if (ImGui::Button("Add Stage"))
    {
        DefaultSetting();        

        auto stages = GameObject::Find("Stages").lock();
        auto stage = NewGameObject("Stage");
        if (stage)
        {
            stage->transform->SetParent(stages->transform);
            stage->AddComponent<Stage>();
            stage->AddComponent<ImageElement>();
            stage->AddComponent<StageView>();
            RegisterStage(*stage.get());
        }
    }

    if (ImGui::Button("Update Data"))
    {
        SetupStage();
    }
}

void MapManager::ChageBackgroundImage(int assetID)
{    
    if (auto rewardPopup = GameObject::Find("RewardPopup").lock(); rewardPopup)
    {
        if (auto background = rewardPopup->transform->Find("Background"); background)
        {
            auto imageElement = background->gameObject->GetComponent<ImageElement>();

            if (imageElement)
            {
                imageElement->SetImage(UmFileSystem.GetGuidFromAssetID(assetID));
            }
        }
    }
}

void MapManager::DefaultSetting()
{
    if (auto uiRoot = GetComponent<UIRoot>(); nullptr == uiRoot)
    {
        AddComponent<UIRoot>();
    }

    if (auto scroll = GameObject::Find("Scroll").lock(); nullptr == scroll)
    {
        scroll = NewGameObject("Scroll");
        scroll->transform->SetParent(gameObject->transform);
        _scroll = &scroll->AddComponent<ScrollingWrapper>();
        scroll->AddComponent<SmoothScroll>();

        auto map = NewGameObject("Map");
        map->transform->SetParent(scroll->transform);
        map->AddComponent<OverlayPanel>().HorizontalFillMode = FillMode::WRAP;

        auto background = NewGameObject("Background");
        background->transform->SetParent(map->transform);
        background->AddComponent<ImageElement>().SetImage(UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[BACKGROUND]));

        auto rewardPopup = NewGameObject("RewardPopup");
        rewardPopup->transform->SetParent(gameObject->transform);
        rewardPopup->AddComponent<RewardPopup>();
        rewardPopup->AddComponent<OverlayPanel>();

        auto rewardBackground = NewGameObject("Background");
        rewardBackground->transform->SetParent(rewardPopup->transform);
        rewardBackground->AddComponent<ImageElement>().SetImage(UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[REWARD_POPUP]));

        auto stages = NewGameObject("Stages");
        stages->AddComponent<OverlayPanel>();
        stages->transform->SetParent(map->transform);
        
        auto stageFocus = NewGameObject("StageFocus");
        stageFocus->transform->SetParent(map->transform);
        auto& imageElement = stageFocus->AddComponent<ImageElement>();
        imageElement.SetImage(UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_FOCUS]));        
        // imageElement.ResetToSpriteSize();
    }
}

void MapManager::SetupStage()
{
    auto stages = GameObject::Find("Stages").lock();

    if (stages)
    {
        const int childCount = stages->transform->GetChildCount();
        for (int i = 0; i < childCount; i++) 
        {
            auto child = stages->transform->GetChild(i);
            if (child)
            {
                RegisterStage(child->gameObject);
            }
        }
    }

    if (auto scroll = GameObject::Find("Scroll").lock(); scroll)
    {
        _scroll = scroll->GetComponent<ScrollingWrapper>();
    }    
}

void MapManager::RegisterStage(GameObject& object)
{
    if (Stage* stage = object.GetComponent<Stage>())
    {
        const int mainLevel = stage->MainLevel;
        const int subLevel  = stage->SubLevel;
        _stages.push_back(stage);
        _stageDataTable[mainLevel][subLevel] = stage;

        std::string key = std::string(object.ToString());
        stage->UpdateData(key, UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_ENABLE]),
                          UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_DISABLE]));

        if (StageView* stageView = object.GetComponent<StageView>())
        {
            stageView->Watch(key);
        }
    }
}

void MapManager::UpdateStageFocus()
{
    for (auto& [mainLevel, stageMap] : _stageDataTable)
    {
        for (auto& [subLevel, stage] : stageMap)
        {
            if (stage)
            {
                bool canSubmit = CanSubmitStage(stage);
                stage->SetEnable(canSubmit);

                const int  stageMapSize      = static_cast<int>(stageMap.size());
                const int  nextFocusSubLevel = std::clamp(_lastClearedStageData.SubLevel, 1, stageMapSize);
                const bool canNextFocus      = stage->SubLevel == nextFocusSubLevel;
                if (canSubmit && canNextFocus)
                {
                    stage->Focus();
                }
            }
        }
    }
}

bool MapManager::CanSubmitStage(Stage* stage)
{
    if (stage)
    {
        return _lastClearedStageData.MainLevel + 1 == stage->MainLevel;
    }
    return false;
}

Stage* MapManager::GetCurrentSelectedStage()
{
    return _selectedStage;
}

Monster::SpawnID MapManager::GetCurrentSpawnID()
{
    if (_selectedStage)
    {
        const int mainLevel   = _selectedStage->MainLevel;
        const int subLevel    = _selectedStage->SubLevel;
        const int battleCount = _selectedStage->BattleCount;
        return Monster::GetSpawnID(mainLevel, subLevel, battleCount);
    }
    return 0;
}

void MapManager::PreferencesKeyDown(const Input::Controller&)
{
    OpenPreferencesWindow();
}

void MapManager::InventoryKeyDown(const Input::Controller&) 
{
    OpenInventoryWindow();
}

void MapManager::ScrollKeyUpdate(const Input::Controller& controller)
{
    Input::Controller::ThumbStickAxis axis = controller.GetRightThumbStickAxis();
    if (PreferencesManager* manager = SingletonComponent<PreferencesManager>::GetInstance())
    {
        if (manager->IsOpen())
        {
            goto label_return;
        }
    } 
    if (InventoryUIManager* manager = SingletonComponent<InventoryUIManager>::GetInstance())
    {
        if (manager->Enable == true)
        {
            goto label_return;
        }
    }
    _scrollDir = -axis.Y * axis.Magnitude;

label_return:
    return;
}

void MapManager::OpenPreferencesWindow()
{
    if (EnableInHierarchy)
    {
        bool isOpen = true;
        if (InventoryUIManager* inventory = SingletonComponent<InventoryUIManager>::GetInstance())
        {
            isOpen = inventory->gameObject->ActiveInHierarchy == false;
        }
        if (isOpen)
        {
            _focusStage.Apply([this](Stage* stage) 
            {
                if (true == stage->EnableInHierarchy)
                {
                    _lastFocusStage = stage;
                    _openPreferences = true;
                }
            });
            UmAudio.Play("-901005");
        }
    }
}

void MapManager::OpenInventoryWindow() 
{
    if (EnableInHierarchy)
    {
        bool isOpen = true;
        if (PreferencesManager* preferences = SingletonComponent<PreferencesManager>::GetInstance())
        {
            isOpen = preferences->IsOpen() == false;
        }
        if (isOpen)
        {
            _focusStage.Apply([this](Stage* stage) 
            {
                if (true == stage->EnableInHierarchy)
                {
                    _lastFocusStage = stage;
                    _openInventory = true;
                }
            });
            UmAudio.Play("-901005");
        }
    }
}
