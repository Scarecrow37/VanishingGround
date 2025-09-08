#include "pchScripts.h"
#include "MapManager.h"

#include "UI/UIRoot/UIRoot.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/Image/ImageElement.h"
#include "UI/Wrappers/Scrolling/ScrollingWrapper.h"

#include "Map/Stage.h"
#include "Map/RewardPopup.h"
#include "Map/SmoothScroll.h"
#include "Map/Views/StageView.h"
#include "Map/ViewModels/StageFocusViewModel.h"
#include "Map/Views/StageFocusView.h"

static GameObject* thisPointer = nullptr;

MapManager::MapManager()
{
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
    if (&gameObject == thisPointer)
    {
        thisPointer = nullptr;
        UmWatcher.Unregister<StageFocusViewModel>("StageFocus");
    }
}

void MapManager::Awake()
{    
    if (nullptr == thisPointer)
    {        
        GameObject::DontDestroyOnLoad(gameObject);
        thisPointer = &gameObject;

        UmWatcher.Register<StageFocusViewModel>("StageFocus", _focusStage);
        SetupStage();
    }
    else
    {        
        GameObject::Destroy(gameObject);
    }
}

void MapManager::Reset()
{
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
        if (ImGui::IsKeyPressed(ImGuiKey_GamepadDpadUp))
        {
            Stage* stage = FindStage(_firstElement + 1, _secondElement);
            if (stage)
            {
                if (stage->IsEnable())
                {
                    _firstElement++;
                    _focusStage = stage;
                }
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_GamepadDpadDown))
        {
            Stage* stage = FindStage(_firstElement - 1, _secondElement);
            if (stage)
            {
                if (stage->IsEnable())
                {
                    _firstElement--;
                    _focusStage = stage;
                }
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_GamepadDpadLeft))
        {
            Stage* stage = FindStage(_firstElement, _secondElement - 1);
            if (stage)
            {
                if (stage->IsEnable())
                {
                    _secondElement--;
                    _focusStage = stage;                    
                }
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_GamepadDpadRight))
        {
            Stage* stage = FindStage(_firstElement, _secondElement + 1);
            if (stage)
            {
                if (stage->IsEnable())
                {
                    _secondElement++;
                    _focusStage = stage;
                }
            }
        }
    }
}

void MapManager::DeserializedReflectEvent()
{
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
    auto background = GameObject::Find("Background").lock();

    if (background)
    {
        auto imageElement = background->GetComponent<ImageElement>();
        if (imageElement)
        {
            imageElement->SetImage(UmFileSystem.GetGuidFromAssetID(assetID));
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
                    stage->UpdateData(key, 
                                      UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_ENABLE]),
                                      UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_DISABLE]));
                }

                auto stageView = child->gameObject->GetComponent<StageView>();
                if (stageView)
                {
                    stageView->Watch(key);
                }

                stage->SetStageEnable(true);
            }
        }
    }

    if (auto scroll = GameObject::Find("Scroll").lock(); scroll)
    {
        _scroll = scroll->GetComponent<ScrollingWrapper>();
    }

    auto map = GameObject::Find("Map").lock();
    if (map)
    {
        auto stageFocus = NewGameObject("StageFocus");
        stageFocus->transform->SetParent(map->transform);
        stageFocus->AddComponent<ImageElement>().SetImage(UmFileSystem.GetGuidFromAssetID(ReflectFields->AssetIDs[STAGE_FOCUS]));
        stageFocus->AddComponent<StageFocusView>().Watch("StageFocus");
    }
}

Stage* MapManager::FindStage(int first, int second)
{
    auto stages = GameObject::Find("Stages").lock();

    if (stages)
    {
        int count = stages->transform->GetChildCount();

        for (int i = 0; i < count; i++)
        {
            auto child = stages->transform->GetChild(i);

            if (child)
            {
                auto stage = child->gameObject->GetComponent<Stage>();

                if (stage)
                {
                    if (stage->GetFirst() == first && stage->GetSecond() == second)
                    {
                        return stage;
                    }
                }
            }
        }
    }

    return nullptr;
}
