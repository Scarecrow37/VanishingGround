#include "pchScripts.h"
#include "Stage.h"
#include "ViewModels/Map/StageViewModel.h"
#include "MapManager.h"
#include "ItemDropSystem/ItemDropSystem.h"

UMREAL_COMPONENT(Stage)

Stage::Stage()
{
    StagePath.SetInputAutoEvent([this]() {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
            {
                DragDropAsset::Data* data      = static_cast<DragDropAsset::Data*>(payLoad->Data);
                File::Path           path      = data->GetPath();
                const auto           extension = path.extension();

                if (extension == L".UmScene")
                {
                    ReflectFields->StagePath = UmFileSystem.GetGuidFromPath(path).string();
                }
            }
            ImGui::EndDragDropTarget();
        }
    });
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

void Stage::FocusIn(FocusCallType callType)
{
    UINavigationComponent::FocusIn(callType);

    if (auto manager = GameObject::Find("MapManager").lock(); manager)
    {
        manager->GetComponent<MapManager>()->SetFocusStage(this);
    }
}

void Stage::Submit()
{
    if (!_stageEnable)
    {
        return;
    }

    std::string stagePath = UmFileSystem.GetPathFromGuid(ReflectFields->StagePath).string();
    if (stagePath.empty())
    {
        return;
    }

    UmSceneManager.LoadScene(stagePath);
    _stageEnable = false;
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
    }
}