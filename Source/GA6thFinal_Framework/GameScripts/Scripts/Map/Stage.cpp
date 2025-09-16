#include "pchScripts.h"
#include "Stage.h"
#include "ViewModels/Map/StageViewModel.h"
#include "MapManager.h"

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

void Stage::Start()
{
    if (ReflectFields->Basefields.get().IsInitialFocus)
    {
        if (auto manager = GameObject::Find("MapManager").lock(); manager)
        {
            manager->GetComponent<MapManager>()->SetFocusStage(this);
        }
    }
}

void Stage::FocusIn()
{
    UINavigationComponent::FocusIn();

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