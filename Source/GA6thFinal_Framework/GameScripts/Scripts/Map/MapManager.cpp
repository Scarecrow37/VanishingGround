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

void MapManager::Awake()
{    
    if (_singletonObject.TrySingleTon(true))
    {        
        _singletonComponent.TrySingleTon();

        UmWatcher.Register<StageFocusViewModel>("StageFocus", _focusStage);
        SetupStage();

        BindInputAction(ControllerButton::BACK, Action::PRESSED, this, &MapManager::PreferencesKeyDown);
    }
}

void MapManager::Update()
{
    if (_scroll)
    {
        if (ImGui::IsKeyDown(ImGuiKey_GamepadRStickUp))
        {
            _scroll->Scroll -= 1.f * UmTime.DeltaTime();
        }
        if (ImGui::IsKeyDown(ImGuiKey_GamepadRStickDown))
        {
            _scroll->Scroll += 1.f * UmTime.DeltaTime();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_F2))
        {
            UmSceneManager.LoadScene(UmFileSystem.GetPathFromGuid(ReflectFields->MapScenePath).string());
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
                    stage->Submit();
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
    if (ImGui::Button("Add Stage"))
    {
        DefaultSetting();        

        auto stages = GameObject::Find("Stages").lock();

        auto stage = NewGameObject("Stage");
        stage->transform->SetParent(stages->transform);

        auto& stageComponent = stage->AddComponent<Stage>();

        std::string key = "Stage" + std::to_string(stages->transform->ChildCount);
        stageComponent.RegisterStage(key, 
                                     UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_ENABLE]),
                                     UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_DISABLE]));

        stage->AddComponent<ImageElement>();
        stage->AddComponent<StageView>().Watch(key);

        _childCount++;
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
        _childCount = stages->transform->GetChildCount();

        for (int i = 0; i < _childCount; i++) 
        {
            auto child = stages->transform->GetChild(i);
            if (child)
            {
                std::string key   = "Stage" + std::to_string(i + 1);
                auto        stage = child->gameObject->GetComponent<Stage>();
                if (stage)
                {
                    _stages.push_back(stage);
                    stage->UpdateData(key, 
                                      UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_ENABLE]),
                                      UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_DISABLE]));
                }

                auto stageView = child->gameObject->GetComponent<StageView>();
                if (stageView)
                {
                    stageView->Watch(key);
                }
            }
        }
    }

    if (auto scroll = GameObject::Find("Scroll").lock(); scroll)
    {
        _scroll = scroll->GetComponent<ScrollingWrapper>();
    }    
}

void MapManager::PreferencesKeyDown(const Input::Controller&) 
{
    OpenPreferencesWindow();
}

void MapManager::InventoryKeyDown(const Input::Controller&) 
{
    OpenInventoryWindow();
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
            isOpen = preferences->gameObject->ActiveInHierarchy == false;
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
        }
    }
}
