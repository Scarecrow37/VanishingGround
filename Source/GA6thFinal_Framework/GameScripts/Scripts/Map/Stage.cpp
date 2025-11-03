#include "pchScripts.h"
#include "Stage.h"
#include "ViewModels/Map/StageViewModel.h"
#include "MapManager.h"
#include "ItemDropSystem/ItemDropSystem.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "Engine/GraphicsCore/RenderPassDataHelper.h"

UMREAL_COMPONENT(Stage)

Stage::Stage()
{
    StagePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data*  data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                const File::Path&           path      = data->GetPath();
                const File::Guid&           guid      = data->GetGuid();
                const File::Path            extension = path.extension();
                if (extension == L".UmScene")
                {
                    ReflectFields->StageGuid = guid.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    LightingPath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                const DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                const File::Path&          path      = data->GetPath();
                const File::Guid&          guid      = data->GetGuid();
                const File::Path           extension = path.extension();
                if (extension == L".inl")
                {
                    ReflectFields->LightingGuid = guid.string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    SetFocusInAudioID("-301000");
    SetSubmitAudioID("-301010");
}

Stage::~Stage()
{
    UmWatcher.Unregister<StageViewModel>(_key);
}

void Stage::RegisterStage(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage)
{
    UmWatcher.Register<StageViewModel>(key, _stageEnable, enableImage, disableImage);
}

void Stage::UpdateData(const std::string& key, const File::Guid& enableImage, const File::Guid& disableImage)
{
    UmWatcher.Unregister<StageViewModel>(key);
    UmWatcher.Register<StageViewModel>(key, _stageEnable, enableImage, disableImage);
    _key = key;
}

void Stage::OnSelected() 
{
    if (auto* transitionComponent = SingletonComponent<SceneTransitionComponent>::GetInstance())
    {
        std::weak_ptr<GameObject> weakOwner = gameObject->GetWeakPtr();
        transitionComponent->SceneTransitionFade("in", "out", [this, weakOwner]() {
            GameObject* owner = weakOwner.lock().get();
            assert(owner && "콜백으로 등록한 객체가 댕글링 포인터입니다.");
            if (owner)
            {
                std::array<DropItemInfo, ARTIFACT_DROP_COUNT>& droptable = _dropItemInfos;

                std::string stagePath   = StagePath;
                std::string lighingPath = LightingPath;
                if (stagePath == File::NULL_PATH)
                {
                    return;
                }
                UmSceneManager.LoadScene(stagePath);
                if (lighingPath != File::NULL_PATH)
                {
                    LoadRenderPassData(lighingPath);
                }
                if (auto instance = SingletonComponent<ItemDropSystem>::GetInstance(); instance)
                {
                    instance->StageClearCount = 0;
                    instance->SetDropItem(droptable);
                }
            }
        });
    }
}

void Stage::FocusIn(FocusCallType callType)
{
    Base::FocusIn(callType);
    if (MapManager* manager = SingletonComponent<MapManager>::GetInstance())
    {
        manager->SetFocusStage(this);
    }
}

void Stage::Submit()
{
    if (MapManager* mapManager = SingletonComponent<MapManager>::GetInstance())
    {
        // 현재 스테이지 선택에 성공하면
        if (mapManager->TrySelectStage(this))
        {
            Base::Submit();
        }
    }
}

void Stage::Start()
{
    if (auto instance = SingletonComponent<ItemDropSystem>::GetInstance(); instance)
    {
        _dropItemInfos = instance->RollArtifacts();

        for (int i = 0; i < ARTIFACT_DROP_COUNT; i++)
        {
            _dropItemAssetIDs[i] = DropItemInfo::GetArtifactCategoryAssetID(_dropItemInfos[i].Category);
        }

        if (MapManager* mapManager = SingletonComponent<MapManager>::GetInstance())
        {
            mapManager->UINotify();
        }
    }
}